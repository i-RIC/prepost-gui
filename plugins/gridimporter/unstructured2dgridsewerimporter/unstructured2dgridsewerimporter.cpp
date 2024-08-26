#include "unstructured2dgridsewerimporter.h"

#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkLine.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>

Unstructured2dGridSewerImporter::Unstructured2dGridSewerImporter() :
	QObject {nullptr},
	GridImporterI{}
{}

QStringList Unstructured2dGridSewerImporter::fileDialogFilters() const
{
	QStringList ret;
	ret << tr("CSV file containing manhole data (*.csv)");
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
	std::unordered_map<std::string, unsigned int> manholeNames;
	std::unordered_map<std::string, std::vector<QVariant> > manholeAtts;
	std::unordered_map<std::string, std::vector<QVariant> > culvertAtts;

	QFileInfo info(filename);

	QString culvertFileName = QFileDialog::getOpenFileName(parent, tr("Select CSV file containing culvert data (*.csv)"), info.absolutePath(), tr("CSV file containing culvert data (*.csv)"));
	if (culvertFileName.isNull()) {return false;}

	auto grid2d = dynamic_cast<v4Unstructured2dGrid*> (grid->grid());

	QFile manholeF(filename);
	manholeF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream manholeTS(&manholeF);

	auto cols = manholeTS.readLine().split(",");
	std::vector<QVariant> empty;
	std::vector<std::string> columnNames;

	for (int i = 0; i < cols.size(); ++i) {
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
			att.push_back(vals.at(i));
		}
		++ pointId;
	}

	manholeF.close();

	QFile culvertF(culvertFileName);
	culvertF.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream culvertTS(&culvertF);

	cols = culvertTS.readLine().split(",");
	columnNames.clear();

	for (int i = 0; i < cols.size(); ++i) {
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
			att.push_back(vals.at(i));
		}
	}

	culvertF.close();

	ugrid->Modified();
	ugrid->BuildLinks();

	grid->allocateAttributes();

	// write attribute values
	auto pd = ugrid->GetPointData();
	for (const auto& pair : manholeAtts) {
		auto array = pd->GetAbstractArray(pair.first.c_str());
		if (array == nullptr) {continue;}

		auto intArray = vtkIntArray::SafeDownCast(array);
		auto doubleArray = vtkDoubleArray::SafeDownCast(array);
		auto stringArray = vtkStringArray::SafeDownCast(array);

		if (intArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				intArray->SetValue(i, pair.second.at(i).toInt());
			}
		} else if (doubleArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				doubleArray->SetValue(i, pair.second.at(i).toDouble());
			}
		} else if (stringArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				stringArray->SetValue(i, iRIC::toStr(pair.second.at(i).toString()));
			}
		}
	}
	auto cd = ugrid->GetCellData();
	for (const auto& pair : culvertAtts) {
		auto array = cd->GetAbstractArray(pair.first.c_str());
		if (array == nullptr) {continue;}

		auto intArray = vtkIntArray::SafeDownCast(array);
		auto doubleArray = vtkDoubleArray::SafeDownCast(array);
		auto stringArray = vtkStringArray::SafeDownCast(array);

		if (intArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				intArray->SetValue(i, pair.second.at(i).toInt());
			}
		} else if (doubleArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				doubleArray->SetValue(i, pair.second.at(i).toDouble());
			}
		} else if (stringArray != nullptr) {
			for (int i = 0; i < pair.second.size(); ++i) {
				stringArray->SetValue(i, iRIC::toStr(pair.second.at(i).toString()));
			}
		}
	}

	return true;
}
