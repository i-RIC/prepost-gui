#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveylandxmlexporter.h"

#include <guibase/landxmlutil.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/mathsupport.h>

#include <QFile>
#include <QXmlStreamWriter>

#include <vtkCell.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>

namespace {

QString numStr(double val)
{
	return QString::number(val, 'f', 3);
}

QString coordStr(double v1, double v2)
{
	return QString("%1 %2").arg(numStr(v1)).arg(numStr(v2));
}

QString coordStr(const QPointF& p)
{
	return coordStr(p.y(), p.x());
}

} // namespace

GeoDataRiverSurveyLandXmlExporter::GeoDataRiverSurveyLandXmlExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("LandXml (*.xml)"), creator)
{}

bool GeoDataRiverSurveyLandXmlExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	auto rsData = dynamic_cast<GeoDataRiverSurvey*>(data);

	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		std::cerr << "Cannot open file for writing: "
							<< qPrintable(file.errorString()) << std::endl;
		return false;
	}

	QXmlStreamWriter writer(&file);

	writer.writeStartDocument("1.0");

	LandXmlUtil::writeStartLandXml(&writer);

	LandXmlUtil::writeProjectForMlit(&writer, rsData->name());
	LandXmlUtil::writeApplication(&writer);
	LandXmlUtil::writeCoordinateSystem(&writer, pd->mainfile()->coordinateSystem());
	LandXmlUtil::writeUnits(&writer);

	writeAlignmentsForLeftDistanceMarkAndCrossSection(&writer, rsData);
	writeAlignmentsForRightDistanceMark(&writer, rsData);
	writeSurfaces(&writer, rsData);

	writer.writeEndElement(); // LandXml
	file.close();

	return true;
}

const QStringList GeoDataRiverSurveyLandXmlExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("LandXml file(*.xml)");
	return ret;
}

void GeoDataRiverSurveyLandXmlExporter::writeAlignmentsForLeftDistanceMarkAndCrossSection(QXmlStreamWriter* writer, GeoDataRiverSurvey* rs)
{
	// calculate length of left bank line
	double len = 0;
	auto point = rs->headPoint()->nextPoint();

	auto prevP = point->leftBank()->interpolate(0);
	while (point->nextPoint() != nullptr) {
		auto p = point->leftBank()->interpolate(1);
		len += iRIC::distance(prevP, p);
		prevP = p;
		point = point->nextPoint();
	}
	// write alignment
	writer->writeStartElement("Alignments");
	writer->writeAttribute("name", "ldm");
	writer->writeStartElement("Alignment");
	writer->writeAttribute("name", "left_distance_mark");
	writer->writeAttribute("length", numStr(len));
	writer->writeAttribute("staStart", "0");

	// write CoordGeom
	point = rs->headPoint()->nextPoint();
	prevP = point->leftBank()->interpolate(0);
	writer->writeStartElement("CoordGeom");
	while (point->nextPoint() != nullptr) {
		auto p = point->leftBank()->interpolate(1);
		writer->writeStartElement("Line");
		writer->writeStartElement("Start");
		writer->writeAttribute("name", QString("%1 L").arg(point->name()));
		writer->writeCharacters(coordStr(prevP));
		writer->writeEndElement(); // Start
		writer->writeStartElement("End");
		writer->writeAttribute("name", QString("%1 L").arg(point->nextPoint()->name()));
		writer->writeCharacters(coordStr(p));
		writer->writeEndElement(); // End
		writer->writeEndElement(); // Line

		prevP = p;
		point = point->nextPoint();
	}
	writer->writeEndElement(); // CoordGeom

	// write Profile
	writer->writeStartElement("Profile");
	writer->writeStartElement("ProfAlign");
	writer->writeAttribute("name", "left_distance_mark_profile");
	writer->writeAttribute("staStart", "0");

	len = 0;
	point = rs->headPoint()->nextPoint();
	prevP = point->leftBank()->interpolate(0);
	double elev = point->crosssection().leftBank(true).height();
	writer->writeStartElement("PVI");
	writer->writeCharacters(coordStr(len, elev));
	writer->writeEndElement(); // PVI
	while (point->nextPoint() != nullptr) {
		auto p = point->leftBank()->interpolate(1);
		len += iRIC::distance(prevP, p);
		double elev = point->crosssection().leftBank(true).height();

		writer->writeStartElement("PVI");
		writer->writeCharacters(coordStr(len, elev));
		writer->writeEndElement(); // PVI
		prevP = p;
		point = point->nextPoint();
	}
	writer->writeEndElement(); // ProfAlign
	writer->writeEndElement(); // Profile

	// write CrossSects
	writer->writeStartElement("CrossSects");
	len = 0;
	point = rs->headPoint()->nextPoint();
	prevP = point->leftBank()->interpolate(0);
	while (point != nullptr) {
		QPointF p;
		if (point->nextPoint() == nullptr) {
			p = point->previousPoint()->leftBank()->interpolate(1);
		} else {
			p = point->leftBank()->interpolate(0);
		}
		len += iRIC::distance(prevP, p);

		writer->writeStartElement("CrossSect");
		writer->writeAttribute("name", point->name());
		writer->writeAttribute("sta", numStr(len));

		writer->writeStartElement("CrossSectSurf");
		writer->writeStartElement("PntList2D");
		QString list = "";
		auto lb = point->crosssection().leftBank(true);
		const auto& alist = point->crosssection().AltitudeInfo();
		bool first = true;
		for (int i = 0; i < alist.size(); ++i) {
			GeoDataRiverCrosssection::Altitude a = alist.at(i);
			if (! a.active()) {continue;}
			double pos = a.position() - lb.position();
			double elev = a.height();

			if (! first) {
				list.append(" ");
			}
			list.append(coordStr(pos, elev));
			first = false;
		}
		writer->writeCharacters(list);

		writer->writeEndElement();// PntList2D
		writer->writeEndElement(); // CrossSectSurf
		writer->writeEndElement(); // CrossSect

		prevP = p;
		point = point->nextPoint();
	}

	writer->writeEndElement(); // CrossSects

	writer->writeEndElement(); // Alignment
	writer->writeEndElement(); // Alignments
}

void GeoDataRiverSurveyLandXmlExporter::writeAlignmentsForRightDistanceMark(QXmlStreamWriter* writer, GeoDataRiverSurvey* rs)
{
	// calculate length of right bank line
	double len = 0;
	auto point = rs->headPoint()->nextPoint();

	auto prevP = point->rightBank()->interpolate(0);
	while (point->nextPoint() != nullptr) {
		auto p = point->rightBank()->interpolate(1);
		len += iRIC::distance(prevP, p);
		prevP = p;
		point = point->nextPoint();
	}
	// write alignment
	writer->writeStartElement("Alignments");
	writer->writeAttribute("name", "rdm");
	writer->writeStartElement("Alignment");
	writer->writeAttribute("name", "right_distance_mark");
	writer->writeAttribute("length", numStr(len));
	writer->writeAttribute("staStart", "0");

	// write CoordGeom
	point = rs->headPoint()->nextPoint();
	prevP = point->rightBank()->interpolate(0);
	writer->writeStartElement("CoordGeom");
	while (point->nextPoint() != nullptr) {
		auto p = point->rightBank()->interpolate(1);
		writer->writeStartElement("Line");
		writer->writeStartElement("Start");
		writer->writeAttribute("name", QString("%1 R").arg(point->name()));
		writer->writeCharacters(coordStr(prevP));
		writer->writeEndElement(); // Start
		writer->writeStartElement("End");
		writer->writeAttribute("name", QString("%1 R").arg(point->nextPoint()->name()));
		writer->writeCharacters(coordStr(p));
		writer->writeEndElement(); // End
		writer->writeEndElement(); // Line

		prevP = p;
		point = point->nextPoint();
	}
	writer->writeEndElement(); // CoordGeom

	writer->writeEndElement(); // Alignment
	writer->writeEndElement(); // Alignments
}

void GeoDataRiverSurveyLandXmlExporter::writeSurfaces(QXmlStreamWriter* writer, GeoDataRiverSurvey* rs)
{
	writer->writeStartElement("Surfaces");
	writer->writeStartElement("Surface");
	writer->writeAttribute("name", rs->name());
	writer->writeStartElement("Definition");
	writer->writeAttribute("surfType", "TIN");

	auto grid = rs->backgroundGrid();
	auto points = grid->GetPoints();
	auto values = vtkDoubleArray::SafeDownCast(grid->GetPointData()->GetArray("Data"));
	double v[3], val;

	writer->writeStartElement("Pnts");
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, v);
		val = values->GetValue(i);

		auto xstr = QString::number(v[0], 'f', 3);
		auto ystr = QString::number(v[1], 'f', 3);
		auto zstr = QString::number(val, 'f', 3);
		writer->writeStartElement("P");
		writer->writeAttribute("id", QString::number(i + 1));
		writer->writeCharacters(QString("%1 %2 %3").arg(ystr).arg(xstr).arg(zstr));
		writer->writeEndElement(); // P
	}
	writer->writeEndElement(); // Pnts

	writer->writeStartElement("Faces");
	for (vtkIdType i = 0; i < grid->GetNumberOfCells(); ++i) {
		auto cell = grid->GetCell(i);
		if (cell->GetNumberOfPoints() == 3) {
			auto id1 = QString::number(cell->GetPointId(0) + 1);
			auto id2 = QString::number(cell->GetPointId(1) + 1);
			auto id3 = QString::number(cell->GetPointId(2) + 1);
			writer->writeStartElement("F");
			writer->writeCharacters(QString("%1 %2 %3").arg(id1).arg(id2).arg(id3));
			writer->writeEndElement(); // F
		} else if (cell->GetNumberOfPoints() == 4) {
			auto id1 = QString::number(cell->GetPointId(0) + 1);
			auto id2 = QString::number(cell->GetPointId(1) + 1);
			auto id3 = QString::number(cell->GetPointId(2) + 1);
			auto id4 = QString::number(cell->GetPointId(3) + 1);
			writer->writeStartElement("F");
			writer->writeCharacters(QString("%1 %2 %3").arg(id1).arg(id2).arg(id3));
			writer->writeEndElement(); // F
			writer->writeStartElement("F");
			writer->writeCharacters(QString("%1 %2 %3").arg(id1).arg(id3).arg(id4));
			writer->writeEndElement(); // F
		}
	}
	writer->writeEndElement(); // Faces

	writer->writeEndElement(); // Definition
	writer->writeEndElement(); // Surface
	writer->writeEndElement(); // Surfaces
}
