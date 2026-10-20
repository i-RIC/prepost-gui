#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveyexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

namespace {

/// Returns the position (distance from the center) that is used as the origin of distances in *.riv files.
/**
 * The origin is the point where "distance from left bank" is zero (-leftShift), so that the
 * "distance from left bank" is kept when the file is imported with the center line CSV file.
 * When that point is on the right side of the center, the left bank is used, because the line
 * between the origin and the right bank must cross the center line.
 */
double distanceOrigin(GeoDataRiverCrosssection& cs)
{
	double origin = - cs.leftShift();
	if (origin > 0) {return cs.leftBank(true).position();}

	return origin;
}

} // namespace

GeoDataRiverSurveyExporter::GeoDataRiverSurveyExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("Cross-Section Data (*.riv)"), creator)
{}

bool GeoDataRiverSurveyExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w, ProjectData* pd)
{
	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		std::cerr << "Cannot open file for writing: "
							<< qPrintable(file.errorString()) << std::endl;
		return false;
	}
	QTextStream outstream(&file);
	outstream.setRealNumberNotation(QTextStream::SmartNotation);
	outstream.setRealNumberPrecision(10);

	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(data);
	auto offset = pd->mainfile()->offset();

	QStringList sectionsWithInactivePoints;
	QStringList sectionsWithExtensions;

	// now, export from the first one.
	GeoDataRiverPathPoint* tmpp = rs->headPoint()->nextPoint();
	outstream << "#survey" << endl;
	while (1) {
		auto& cs = tmpp->crosssection();
		QPointF leftBank  = tmpp->crosssectionPosition(distanceOrigin(cs));
		QPointF rightBank = tmpp->crosssectionPosition(cs.rightBank(true).position());
		outstream
				<< tmpp->name()
				<< "\t" << leftBank.x() + offset.x() << "\t" << leftBank.y() + offset.y()
				<< "\t" << rightBank.x() + offset.x() << "\t" << rightBank.y()  + offset.y() << endl;
		if (cs.numOfAltitudes(true) != cs.numOfAltitudes(false)) {sectionsWithInactivePoints.push_back(tmpp->name());}
		if (cs.fixedPointLSet() || cs.fixedPointRSet()) {sectionsWithExtensions.push_back(tmpp->name());}
		tmpp = tmpp->nextPoint();
		if (tmpp == nullptr) {break;}
	}

	// now export crosssection point.
	tmpp = rs->headPoint()->nextPoint();
	outstream << endl << endl << "#x-section" << endl;
	while (1) {
		outstream
				<< tmpp->name() << "\t" << tmpp->crosssection().numOfAltitudes(true) << endl;
		int i = 0;
		double left = distanceOrigin(tmpp->crosssection());
		for (GeoDataRiverCrosssection::Altitude& alt : tmpp->crosssection().AltitudeInfo()) {
			if (alt.active()) {
				outstream << alt.position() - left << "\t" << alt.height() << "\t";
			}
			++i;
			if (i == 5) {
				outstream << endl;
				i = 0;
			}
		}
		outstream << endl;
		tmpp = tmpp->nextPoint();
		if (tmpp == nullptr) {break;}
	}
	file.close();

	// export centerpoints
	auto csvFilename = filename;
	csvFilename.append(".csv"); // @todo check and fix the name.
	QFile csvFile(csvFilename);
	if (! csvFile.open(QIODevice::WriteOnly)) {
		return false;
	}
	QTextStream csvOutstream(&csvFile);
	csvOutstream.setRealNumberNotation(QTextStream::SmartNotation);
	csvOutstream.setRealNumberPrecision(10);
	csvOutstream << "x" << "," << "y" << "," << "name" << endl;

	tmpp = rs->headPoint()->nextPoint();
	while (true) {
		QPointF center = tmpp->position();
		csvOutstream << center.x() + offset.x() << "," << center.y() + offset.y() << "," << tmpp->name() << endl;

		tmpp = tmpp->nextPoint();
		if (tmpp == nullptr) {break;}
	}
	csvFile.close();

	if (w != nullptr && (sectionsWithInactivePoints.size() > 0 || sectionsWithExtensions.size() > 0)) {
		QStringList msgs;
		if (sectionsWithInactivePoints.size() > 0) {
			msgs.push_back(tr("Inactive points are not exported, because *.riv files can not hold them. Cross sections with inactive points: %1").arg(sectionsWithInactivePoints.join(", ")));
		}
		if (sectionsWithExtensions.size() > 0) {
			msgs.push_back(tr("Extension lines are exported as straight lines, because *.riv files can not hold them. Cross sections with extension lines: %1").arg(sectionsWithExtensions.join(", ")));
		}
		QMessageBox::warning(w, tr("Warning"), msgs.join("\n\n"));
	}

	return true;
}

const QStringList GeoDataRiverSurveyExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Cross-Section Data (*.riv)");
	return ret;
}
