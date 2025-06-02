#ifndef GEODATARIVERSURVEYJMKIMPORTER_H
#define GEODATARIVERSURVEYJMKIMPORTER_H

#include <QObject>

class GeoDataRiverSurvey;

class GeoDataRiverSurveyJmkImporter : public QObject
{
	Q_OBJECT

public:
	GeoDataRiverSurveyJmkImporter();

	bool import(const QString& filename, GeoDataRiverSurvey* rs, QWidget* w);
};

#endif // GEODATARIVERSURVEYJMKIMPORTER_H
