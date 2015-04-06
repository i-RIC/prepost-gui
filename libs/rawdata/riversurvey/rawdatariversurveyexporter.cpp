#include "rawdatarivercrosssection.h"
#include "rawdatariverpathpoint.h"
#include "rawdatariversurvey.h"
#include "rawdatariversurveyexporter.h"
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QFile>
#include <QTextStream>

RawDataRiverSurveyExporter::RawDataRiverSurveyExporter(RawDataCreator* creator)
	: RawDataExporter(creator)
{
	m_caption = tr("River Survey data (*.riv)");
}

bool RawDataRiverSurveyExporter::doEmport(RawData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)){
		std::cerr << "Cannot open file for writing: "
							<< qPrintable(file.errorString()) << std::endl;
		return false;
	}
	QTextStream outstream(&file);
	outstream.setRealNumberNotation(QTextStream::SmartNotation);
	outstream.setRealNumberPrecision(10);

	RawDataRiverSurvey* rs = dynamic_cast<RawDataRiverSurvey*>(data);
	RawDataRiverPathPoint* lastp = rs->headPoint()->nextPoint();
	QVector2D offset = pd->mainfile()->offset();
	while (1){
		if (lastp->nextPoint() == 0){break;}
		lastp = lastp->nextPoint();
	}
	// now, export from the last one.
	RawDataRiverPathPoint* tmpp = lastp;
	outstream << "#survey" << endl;
	while (1){
		QVector2D leftBank  = tmpp->crosssectionPosition(tmpp->crosssection().leftBank(true).position());
		QVector2D rightBank = tmpp->crosssectionPosition(tmpp->crosssection().rightBank(true).position());
		outstream
		<< tmpp->name()
		<< "\t" << leftBank.x() + offset.x() << "\t" << leftBank.y() + offset.y()
		<< "\t" << rightBank.x() + offset.x() << "\t" << rightBank.y()  + offset.y() << endl;
		tmpp = tmpp->previousPoint();
		if (tmpp->firstPoint()){break;}
	}
	// now export crosssection point.

	tmpp = lastp;
	outstream << endl << endl << "#x-section" << endl;
	while (1){
		outstream
		<< tmpp->name() << "\t" << tmpp->crosssection().numOfAltitudes(true) << endl;
		int i = 0;
		double left = tmpp->crosssection().leftBank(true).position();
		const RawDataRiverCrosssection::AltitudeList& list = tmpp->crosssection().AltitudeInfo();
		RawDataRiverCrosssection::AltitudeList::const_iterator it;
		for (it = list.begin(); it != list.end(); ++it){
			if (it->active()){
				outstream << it->position() - left << "\t" << it->height() << "\t";
			}
			++i;
			if (i == 5){
				outstream << endl;
				i = 0;
			}
		}
		outstream << endl;
		tmpp = tmpp->previousPoint();
		if (tmpp->firstPoint()){break;}
	}
	file.close();
	return true;
}

const QStringList RawDataRiverSurveyExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("River Survey data(*.riv)");
	return ret;
}
