#include "unstructured2dgridsewerimporter.h"

#include <geoio/dbfcloser.h>
#include <geoio/shpcloser.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <misc/qpointfcompare.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextCodec>
#include <QTextStream>

#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkSmartPointer.h>

#include <shapefil.h>

#include <map>

namespace {

bool importFromCsvFile(v4InputGrid* grid, const QString& manholeFilename, QWidget* parent)
{
	auto grid2d = dynamic_cast<v4Unstructured2dGrid*> (grid->grid());

	std::unordered_map<std::string, unsigned int> manholeNames;
	std::unordered_map<std::string, std::vector<QVariant> > manholeAtts;
	std::unordered_map<std::string, std::vector<QVariant> > culvertAtts;

	QFileInfo info(manholeFilename);

	QString culvertFilename = QFileDialog::getOpenFileName(parent, Unstructured2dGridSewerImporter::tr("Select file containing culvert data"), info.absolutePath(), Unstructured2dGridSewerImporter::tr("CSV file containing culvert data (*.csv)"));
	if (culvertFilename.isNull()) {return false;}

	QFile manholeF(manholeFilename);
	manholeF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream manholeTS(&manholeF);

	auto cols = manholeTS.readLine().split(",");
	std::vector<QVariant> empty;
	std::vector<std::string> columnNames;

	for (int i = 3; i < cols.size(); ++i) {
		auto name = iRIC::toStr(cols.at(i));
		manholeAtts.insert({name, empty});
		columnNames.push_back(name);
	}

	vtkUnstructuredGrid* ugrid = grid2d->vtkConcreteData()->concreteData();
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	ugrid->SetPoints(points);
	unsigned pointId = 0;
	while (! manholeTS.atEnd()) {
		auto vals = manholeTS.readLine().split(",");
		if (vals.size() != cols.size()) {continue;}

		auto x = vals.at(0).toDouble();
		auto y = vals.at(1).toDouble();
		auto name = iRIC::toStr(vals.at(2));
		points->InsertNextPoint(x, y, 0);
		manholeNames.insert({name, pointId});

		for (unsigned int i = 0; i < columnNames.size(); ++i) {
			auto &att = manholeAtts.at(columnNames.at(i));
			att.push_back(vals.at(i + 3));
		}
		++ pointId;
	}

	manholeF.close();

	QFile culvertF(culvertFilename);
	culvertF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream culvertTS(&culvertF);

	cols = culvertTS.readLine().split(",");
	columnNames.clear();

	for (int i = 3; i < cols.size(); ++i) {
		auto name = iRIC::toStr(cols.at(i));
		culvertAtts.insert({name, empty});
		columnNames.push_back(name);
	}

	auto line = vtkSmartPointer<vtkLine>::New();
	while (! culvertTS.atEnd()) {
		auto vals = culvertTS.readLine().split(",");
		if (vals.size() != cols.size()) {continue;}

		auto name = iRIC::toStr(vals.at(0));
		auto manholeEName = iRIC::toStr(vals.at(1));
		auto manholeSName = iRIC::toStr(vals.at(2));
		auto e_it = manholeNames.find(manholeEName);
		auto s_it = manholeNames.find(manholeSName);

		// TODO add error handling
		if (e_it == manholeNames.end()) {continue;}
		if (s_it == manholeNames.end()) {continue;}

		line->GetPointIds()->SetId(0, s_it->second);
		line->GetPointIds()->SetId(1, e_it->second);
		ugrid->InsertNextCell(line->GetCellType(), line->GetPointIds());

		for (unsigned int i = 0; i < columnNames.size(); ++i) {
			auto &att = culvertAtts.at(columnNames.at(i));
			att.push_back(vals.at(i + 3));
		}
	}

	culvertF.close();

	ugrid->Modified();
	ugrid->BuildLinks();

	grid->allocateAttributes();

	// write attribute values
	auto pd = ugrid->GetPointData();
	for (const auto& pair : manholeAtts) {
		auto array = pd->GetArray(pair.first.c_str());
		if (array == nullptr) {continue;}

		auto intArray = vtkIntArray::SafeDownCast(array);
		auto doubleArray = vtkDoubleArray::SafeDownCast(array);

		if (intArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				intArray->SetValue(i, pair.second.at(i).toInt());
			}
		} else if (doubleArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				doubleArray->SetValue(i, pair.second.at(i).toDouble());
			}
		}
	}
	auto cd = ugrid->GetCellData();
	for (const auto& pair : culvertAtts) {
		auto array = cd->GetArray(pair.first.c_str());
		if (array == nullptr) {continue;}

		auto intArray = vtkIntArray::SafeDownCast(array);
		auto doubleArray = vtkDoubleArray::SafeDownCast(array);

		if (intArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				intArray->SetValue(i, pair.second.at(i).toInt());
			}
		} else if (doubleArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				doubleArray->SetValue(i, pair.second.at(i).toDouble());
			}
		}
	}

	return true;
}

QVariant readShapefileData(DBFHandle handle, int dataid, int fieldid, QTextCodec* codec)
{
	DBFFieldType type = DBFGetFieldInfo(handle, fieldid, NULL, NULL, NULL);
	QVariant val;
	if (type == FTString) {
		QString strval = codec->toUnicode(DBFReadStringAttribute(handle, dataid, fieldid));
		val = strval;
	} else if (type == FTInteger) {
		int intval = DBFReadIntegerAttribute(handle, dataid, fieldid);
		val = intval;
	} else if (type == FTDouble) {
		double doubleval = DBFReadDoubleAttribute(handle, dataid, fieldid);
		val = doubleval;
	} else if (type == FTLogical) {
		QString logval = DBFReadLogicalAttribute(handle, dataid, fieldid);
		val = logval;
	}
	return val;
}

bool importFromShapefile(v4InputGrid* grid, QString manholeFilename, QWidget* parent)
{
	QTextCodec* codec = QTextCodec::codecForName("utf-8");

	auto grid2d = dynamic_cast<v4Unstructured2dGrid*> (grid->grid());

	std::unordered_map<std::string, unsigned int> manholeNames;
	std::unordered_map<std::string, std::vector<QVariant> > manholeAtts;
	std::unordered_map<std::string, std::vector<QVariant> > culvertAtts;

	QFileInfo info(manholeFilename);

	QString culvertFilename = QFileDialog::getOpenFileName(parent, Unstructured2dGridSewerImporter::tr("Select file containing culvert data"), info.absolutePath(), Unstructured2dGridSewerImporter::tr("Shapefile containing culvert data (*.shp)"));
	if (culvertFilename.isNull()) {return false;}

	SHPHandle manholeShph = SHPOpen(iRIC::toStr(manholeFilename).c_str(), "rb");
	SHPCloser mahholeShphCloser(manholeShph);

	int numEntities;
	int shapeType;
	double minBound[4];
	double maxBound[4];

	SHPGetInfo(manholeShph, &numEntities, &shapeType, minBound, maxBound);
	if (shapeType != SHPT_POINT) {
		QMessageBox::critical(parent, Unstructured2dGridSewerImporter::tr("Error"), Unstructured2dGridSewerImporter::tr("The shape type contained in %1 is not point.").arg(manholeFilename));
		return false;
	}

	auto manholeDbfFilename = manholeFilename;
	manholeDbfFilename.replace(QRegExp(".shp$"), ".dbf");

	DBFHandle manholeDbfh = DBFOpen(iRIC::toStr(manholeDbfFilename).c_str(), "rb");
	DBFCloser manholeDbfhCloser(manholeDbfh);

	int recordCount = DBFGetRecordCount(manholeDbfh);

	if (numEntities != recordCount) {
		// wrong shape file.
		QMessageBox::critical(parent, Unstructured2dGridSewerImporter::tr("Error"), Unstructured2dGridSewerImporter::tr("The number of data mismatches between shp file and dbf file."));
		return false;
	}

	auto ugrid = grid2d->vtkConcreteData()->concreteData();
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	std::map<QPointF, int, QPointFCompare> pointIdMap;

	for (int i = 0; i < numEntities; ++i) {
		SHPObject* shpo = SHPReadObject(manholeShph, i);
		double x = *(shpo->padfX);
		double y = *(shpo->padfY);

		points->InsertNextPoint(x, y, 0);
		QPointF point(x, y);
		pointIdMap.insert({point, i});

		SHPDestroyObject(shpo);
	}
	ugrid->SetPoints(points);

	int fieldCount = DBFGetFieldCount(manholeDbfh);

	std::vector<QVariant> empty;
	std::vector<std::string> columnNames;

	for (int i = 0; i < fieldCount; ++i) {
		DBFFieldType type;
		char fieldName[12];
		type = DBFGetFieldInfo(manholeDbfh, i, fieldName, NULL, NULL);
		manholeAtts.insert({fieldName, empty});
		columnNames.push_back(fieldName);
	}

	for (int i = 0; i < recordCount; ++i) {
		for (int j = 0; j < fieldCount; ++j) {
			auto data = readShapefileData(manholeDbfh, i, j, codec);
			manholeAtts.at(columnNames[j]).push_back(data);
		}
	}

	SHPHandle culvertShph = SHPOpen(iRIC::toStr(culvertFilename).c_str(), "rb");
	SHPCloser culvertShphCloser(culvertShph);

	SHPGetInfo(culvertShph, &numEntities, &shapeType, minBound, maxBound);
	if (shapeType != SHPT_ARC) {
		QMessageBox::critical(parent, Unstructured2dGridSewerImporter::tr("Error"), Unstructured2dGridSewerImporter::tr("The shape type contained in %1 is not line.").arg(manholeFilename));
		return false;
	}

	auto culvertDbfFilename = culvertFilename;
	culvertDbfFilename.replace(QRegExp(".shp$"), ".dbf");

	DBFHandle culvertDbfh = DBFOpen(iRIC::toStr(culvertDbfFilename).c_str(), "rb");
	DBFCloser culvertDbfhCloser(culvertDbfh);

	recordCount = DBFGetRecordCount(culvertDbfh);

	if (numEntities != recordCount) {
		// wrong shape file.
		QMessageBox::critical(parent, Unstructured2dGridSewerImporter::tr("Error"), Unstructured2dGridSewerImporter::tr("The number of data mismatches between shp file and dbf file."));
		return false;
	}

	bool invalidLineExists = false;

	auto line = vtkSmartPointer<vtkLine>::New();
	for (int i = 0; i < numEntities; ++i) {
		SHPObject* shpo = SHPReadObject(culvertShph, i);
		if (shpo->nVertices != 2) {
			invalidLineExists = true;
			continue;
		}
		double x1 = *(shpo->padfX);
		double y1 = *(shpo->padfY);

		double x2 = *(shpo->padfX + 1);
		double y2 = *(shpo->padfY + 1);

		auto it1 = pointIdMap.find(QPointF(x1, y1));
		auto it2 = pointIdMap.find(QPointF(x2, y2));
		if (it1 == pointIdMap.end() || it2 == pointIdMap.end()) {
			invalidLineExists = true;
			continue;
		}
		line->GetPointIds()->SetId(0, it1->second);
		line->GetPointIds()->SetId(1, it2->second);

		ugrid->InsertNextCell(line->GetCellType(), line->GetPointIds());

		SHPDestroyObject(shpo);
	}

	fieldCount = DBFGetFieldCount(culvertDbfh);
	columnNames.clear();

	for (int i = 0; i < fieldCount; ++i) {
		DBFFieldType type;
		char fieldName[12];
		type = DBFGetFieldInfo(culvertDbfh, i, fieldName, NULL, NULL);
		culvertAtts.insert({fieldName, empty});
		columnNames.push_back(fieldName);
	}

	for (int i = 0; i < recordCount; ++i) {
		for (int j = 0; j < fieldCount; ++j) {
			auto data = readShapefileData(culvertDbfh, i, j, codec);
			culvertAtts.at(columnNames[j]).push_back(data);
		}
	}

	ugrid->Modified();
	ugrid->BuildLinks();

	grid->allocateAttributes();

	// write attribute values
	auto pd = ugrid->GetPointData();
	for (const auto& pair : manholeAtts) {
		auto array = pd->GetArray(pair.first.c_str());
		if (array == nullptr) {continue;}

		auto intArray = vtkIntArray::SafeDownCast(array);
		auto doubleArray = vtkDoubleArray::SafeDownCast(array);

		if (intArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				intArray->SetValue(i, pair.second.at(i).toInt());
			}
		} else if (doubleArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				doubleArray->SetValue(i, pair.second.at(i).toDouble());
			}
		}
	}
	auto cd = ugrid->GetCellData();
	for (const auto& pair : culvertAtts) {
		auto array = cd->GetArray(pair.first.c_str());
		if (array == nullptr) {continue;}

		auto intArray = vtkIntArray::SafeDownCast(array);
		auto doubleArray = vtkDoubleArray::SafeDownCast(array);

		if (intArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				intArray->SetValue(i, pair.second.at(i).toInt());
			}
		} else if (doubleArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				doubleArray->SetValue(i, pair.second.at(i).toDouble());
			}
		}
	}

	return true;
}

} // namespace

Unstructured2dGridSewerImporter::Unstructured2dGridSewerImporter() :
	QObject {nullptr},
	GridImporterI{}
{}

QStringList Unstructured2dGridSewerImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("CSV file containing manhole data (*.csv)");
	ret << tr("Shapefile containing manhole data (*.shp)");
	return ret;
}

QString Unstructured2dGridSewerImporter::caption() const
{
	return tr("Sewer data");
}

SolverDefinitionGridType::GridType Unstructured2dGridSewerImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtUnstructured2DGrid;
}

bool Unstructured2dGridSewerImporter::import(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	QFileInfo info(filename);

	if (info.suffix() == "csv") {
		return importFromCsvFile(grid, filename, parent);
	} else if (info.suffix() == "shp") {
		return importFromShapefile(grid, filename, parent);
	}

	return false;
}
