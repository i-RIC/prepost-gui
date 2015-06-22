#include "hydraulicdatariversurveywaterelevationimporter.h"

#include <geodata/riversurvey/geodatariversurvey.h>

#include <QFile>
#include <QMap>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>

HydraulicDataRiverSurveyWaterElevationImporter::HydraulicDataRiverSurveyWaterElevationImporter()
{
	m_caption = tr("Water Elevation");
}

bool HydraulicDataRiverSurveyWaterElevationImporter::import(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w)
{
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(data);

	QFile file(filename);
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(filename));
		return false;
	}

	QMap<double, GeoDataRiverPathPoint*> pmap;
	GeoDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	while (p != nullptr) {
		double name = p->name().toDouble();
		pmap.insert(name, p);
		p = p->nextPoint();
	}
	QTextStream in(&file);
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
			GeoDataRiverPathPoint* point = pmap.value(kp, 0);
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

bool HydraulicDataRiverSurveyWaterElevationImporter::canImportTo(GeoData* data)
{
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(data);
	return (rs != nullptr);
}

const QStringList HydraulicDataRiverSurveyWaterElevationImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("CSV file (*.csv)"));
	ret.append(tr("Text file (*.txt)"));
	return ret;
}
