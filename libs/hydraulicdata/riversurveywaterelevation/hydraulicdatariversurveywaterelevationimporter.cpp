#include "hydraulicdatariversurveywaterelevationimporter.h"
#include <rawdata/riversurvey/rawdatariversurvey.h>

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QMessageBox>

HydraulicDataRiverSurveyWaterElevationImporter::HydraulicDataRiverSurveyWaterElevationImporter()
{
	m_caption = tr("Water Elevation");
}

bool HydraulicDataRiverSurveyWaterElevationImporter::import(RawData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w)
{
	RawDataRiverSurvey* rs = dynamic_cast<RawDataRiverSurvey*>(data);

	QFile file(filename);
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(filename));
		return false;
	}

	QMap<double, RawDataRiverPathPoint*> pmap;
	RawDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	while (p != nullptr) {
		double name = p->name().toDouble();
		pmap.insert(name, p);
		p = p->nextPoint();
	}
	QTextStream in(&file);
	double kp;
	double height;

	QStringList skipped;

	// skip the first line (header).
	QString header;
	header = in.readLine();
	QString line;
	do {
		line = in.readLine();
		if (! line.isEmpty()) {
			QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::SkipEmptyParts);
			double kp = pieces.value(0).toDouble();
			double height = pieces.value(1).toDouble();
			RawDataRiverPathPoint* point = pmap.value(kp, 0);
			if (point == nullptr) {
				skipped.append(QString::number(kp));
			} else {
				point->setWaterSurfaceElevation(height);
			}
		}
	} while (! line.isEmpty());
	file.close();

	if (skipped.size() > 0) {
		QMessageBox::warning(w, tr("Warning"), tr("Data with KP values %1 are ignored.").arg(skipped.join(", ")));
	}
	return true;
}

bool HydraulicDataRiverSurveyWaterElevationImporter::canImportTo(RawData* data)
{
	RawDataRiverSurvey* rs = dynamic_cast<RawDataRiverSurvey*>(data);
	return (rs != nullptr);
}

const QStringList HydraulicDataRiverSurveyWaterElevationImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("CSV file (*.csv)"));
	ret.append(tr("Text file (*.txt)"));
	return ret;
}
