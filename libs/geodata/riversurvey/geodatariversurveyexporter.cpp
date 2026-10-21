#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveyexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <algorithm>
#include <limits>

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
	outstream.setRealNumberPrecision(std::numeric_limits<double>::max_digits10);

	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(data);
	auto offset = pd->mainfile()->offset();

	QStringList sectionsWithInactivePoints;
	QStringList sectionsWithExtensions;

	// now, export from the first one.
	GeoDataRiverPathPoint* tmpp = rs->headPoint()->nextPoint();
	outstream << "#survey" << endl;
	while (1) {
		auto& cs = tmpp->crosssection();
		const double origin = - cs.leftShift();
		double right = cs.rightBank(true).position();
		if (right <= origin) {
			// Keep the reference direction left-to-right even when all distances are negative.
			right = origin + std::max(1.0, cs.rightBank(true).position() - cs.leftBank(true).position());
		}
		QPointF leftBank  = tmpp->crosssectionPosition(origin);
		QPointF rightBank = tmpp->crosssectionPosition(right);
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
		double left = - tmpp->crosssection().leftShift();
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
	csvOutstream.setRealNumberPrecision(std::numeric_limits<double>::max_digits10);
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
