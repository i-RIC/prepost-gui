#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveyexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QFile>
#include <QTextStream>

GeoDataRiverSurveyExporter::GeoDataRiverSurveyExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("River Survey data (*.riv)"), creator)
{}

bool GeoDataRiverSurveyExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
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
	GeoDataRiverPathPoint* lastp = rs->headPoint()->nextPoint();
	auto offset = pd->mainfile()->offset();

	// now, export from the first one.
	GeoDataRiverPathPoint* tmpp = rs->headPoint()->nextPoint();
	outstream << "#survey" << endl;
	while (1) {
		QPointF leftBank  = tmpp->crosssectionPosition(tmpp->crosssection().leftBank(true).position());
		QPointF rightBank = tmpp->crosssectionPosition(tmpp->crosssection().rightBank(true).position());
		outstream
				<< tmpp->name()
				<< "\t" << leftBank.x() + offset.x() << "\t" << leftBank.y() + offset.y()
				<< "\t" << rightBank.x() + offset.x() << "\t" << rightBank.y()  + offset.y() << endl;
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
		double left = tmpp->crosssection().leftBank(true).position();
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
	return true;
}

const QStringList GeoDataRiverSurveyExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("River Survey data(*.riv)");
	return ret;
}
