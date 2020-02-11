#include "geodatapointmaplandxmlexporter.h"
#include "geodatapointmapt.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guibase/landxmlutil.h>

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamWriter>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>

GeoDataPointmapLandXmlExporter::GeoDataPointmapLandXmlExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("LandXML"), creator)
{}

bool GeoDataPointmapLandXmlExporter::doExport(GeoData* data, const QString& filename, const QString&, QWidget* w, ProjectData* pd)
{
	auto pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*> (data);
	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while exporting to %1").arg(QDir::toNativeSeparators(filename)));
		return false;
	}
	auto polydata = pmap->delaunayedPolyData();

	QXmlStreamWriter writer(&file);
	writer.writeStartDocument("1.0");

	LandXmlUtil::writeStartLandXml(&writer);

	LandXmlUtil::writeProjectForMlit(&writer, pmap->name());
	LandXmlUtil::writeApplication(&writer);
	LandXmlUtil::writeCoordinateSystem(&writer, pd->mainfile()->coordinateSystem());
	LandXmlUtil::writeUnits(&writer);

	writer.writeStartElement("Surfaces");
	writer.writeStartElement("Surface");
	writer.writeAttribute("name", pmap->name());
	writer.writeStartElement("Definition");
	writer.writeAttribute("surfType", "TIN");

	auto offset = pd->mainfile()->offset();
	auto points = polydata->GetPoints();
	auto values = vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray("values"));
	double v[3], val;

	writer.writeStartElement("Pnts");
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, v);
		val = values->GetValue(i);
		v[0] += offset.x();
		v[1] += offset.y();

		auto xstr = QString::number(v[0], 'f', 3);
		auto ystr = QString::number(v[1], 'f', 3);
		auto zstr = QString::number(val, 'f', 3);
		writer.writeStartElement("P");
		writer.writeAttribute("id", QString::number(i + 1));
		writer.writeCharacters(QString("%1 %2 %3").arg(ystr).arg(xstr).arg(zstr));
		writer.writeEndElement(); // P
	}
	writer.writeEndElement(); // Pnts

	writer.writeStartElement("Faces");
	vtkIdList* idlist = vtkIdList::New();
	for (vtkIdType i = 0; i < polydata->GetNumberOfCells(); ++i) {
		auto cell = polydata->GetCell(i);
		auto id1 = cell->GetPointId(0) + 1;
		auto id2 = cell->GetPointId(1) + 1;
		auto id3 = cell->GetPointId(2) + 1;
		writer.writeStartElement("F");
		writer.writeCharacters(QString("%1 %2 %3").arg(id1).arg(id2).arg(id3));
		writer.writeEndElement(); // F
	}
	idlist->Delete();
	writer.writeEndElement(); // Faces

	writer.writeEndElement(); // Definition
	writer.writeEndElement(); // Surface
	writer.writeEndElement(); // Surfaces
	writer.writeEndElement(); // LandXML

	writer.writeEndDocument();
	file.close();
	return true;
}

const QStringList GeoDataPointmapLandXmlExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("LandXML file (*.xml)");
	return ret;
}
