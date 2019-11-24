#include "gridlandxmlexporter.h"

#include <guibase/landxmlutil.h>
#include <guicore/pre/grid/grid.h>
#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QXmlStreamWriter>

#include <vtkCell.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPointSet.h>
#include <vtkPoints.h>

GridLandXmlExporter::GridLandXmlExporter() :
	QObject()
{}

GridLandXmlExporter::~GridLandXmlExporter()
{}

QString GridLandXmlExporter::caption() const
{
	return tr("LandXml file");
}

bool GridLandXmlExporter::isGridTypeSupported(SolverDefinitionGridType::GridType gt) const
{
	return (gt == SolverDefinitionGridType::gtStructured2DGrid) || (gt == SolverDefinitionGridType::gtUnstructured2DGrid);
}

QStringList GridLandXmlExporter::fileDialogFilters() const
{
	QStringList ret;
	ret << QObject::tr("LandXml files (*.xml)");
	return ret;
}

bool GridLandXmlExporter::doExport(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(parent, tr("Error"), tr("Error occured while exporting to %1").arg(QDir::toNativeSeparators(filename)));
		return false;
	}
	auto vtkGrid = grid->vtkGrid();

	QXmlStreamWriter writer(&file);
	writer.writeStartDocument("1.0");

	LandXmlUtil::writeStartLandXml(&writer);

	LandXmlUtil::writeProjectForMlit(&writer, "grid");
	LandXmlUtil::writeApplication(&writer);
	// @todo CoordinateSystem
	LandXmlUtil::writeUnits(&writer);

	writer.writeStartElement("Surfaces");
	writer.writeStartElement("Surface");
	writer.writeAttribute("name", "grid");
	writer.writeStartElement("Definition");
	writer.writeAttribute("surfType", "TIN");

	auto points = vtkGrid->GetPoints();
	vtkDoubleArray* elevArray = nullptr;

	auto pointData = vtkGrid->GetPointData();
	for (int i = 0; i < pointData->GetNumberOfArrays(); ++i) {
		QString name(pointData->GetArrayName(i));
		if (name.contains("elevation", Qt::CaseInsensitive)) {
			elevArray = vtkDoubleArray::SafeDownCast(pointData->GetArray(i));
		}
	}

	double v[3], val;
	val = 0;
	writer.writeStartElement("Pnts");
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, v);
		if (elevArray != nullptr) {
			val = elevArray->GetValue(i);
		}
		auto xstr = QString::number(v[0], 'f', 3);
		auto ystr = QString::number(v[1], 'f', 3);
		auto zstr = QString::number(val, 'f', 3);
		writer.writeStartElement("P");
		writer.writeAttribute("id", QString::number(i + 1));
		writer.writeCharacters(QString("%1 %2 %3").arg(xstr).arg(ystr).arg(zstr));
		writer.writeEndElement(); // P
	}
	writer.writeEndElement(); // Pnts

	writer.writeStartElement("Faces");
	for (vtkIdType i = 0; i < vtkGrid->GetNumberOfCells(); ++i) {
		auto cell = vtkGrid->GetCell(i);
		if (cell->GetNumberOfPoints() == 3) {
			auto id1 = QString::number(cell->GetPointId(0) + 1);
			auto id2 = QString::number(cell->GetPointId(1) + 1);
			auto id3 = QString::number(cell->GetPointId(2) + 1);
			writer.writeStartElement("F");
			writer.writeCharacters(QString("%1 %2 %3").arg(id1).arg(id2).arg(id3));
			writer.writeEndElement(); // F
		} else if (cell->GetNumberOfPoints() == 4) {
			auto id1 = QString::number(cell->GetPointId(0) + 1);
			auto id2 = QString::number(cell->GetPointId(1) + 1);
			auto id3 = QString::number(cell->GetPointId(2) + 1);
			auto id4 = QString::number(cell->GetPointId(3) + 1);
			writer.writeStartElement("F");
			writer.writeCharacters(QString("%1 %2 %3").arg(id1).arg(id2).arg(id3));
			writer.writeEndElement(); // F
			writer.writeStartElement("F");
			writer.writeCharacters(QString("%1 %2 %3").arg(id1).arg(id3).arg(id4));
			writer.writeEndElement(); // F
		}
	}

	writer.writeEndElement(); // Definition
	writer.writeEndElement(); // Surface
	writer.writeEndElement(); // Surfaces
	writer.writeEndElement(); // LandXML

	writer.writeEndDocument();

	return true;
}
