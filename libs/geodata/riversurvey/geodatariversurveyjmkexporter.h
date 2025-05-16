#ifndef GEODATARIVERSURVEYJMKEXPORTER_H
#define GEODATARIVERSURVEYJMKEXPORTER_H

#include <QObject>

class GeoDataRiverSurvey;

class GeoDataRiverSurveyJmkExporter: public QObject
{
	Q_OBJECT

public:
	GeoDataRiverSurveyJmkExporter();

	bool doExport(const QString& filename, GeoDataRiverSurvey* rs, QWidget* w);

	static bool check(GeoDataRiverSurvey* rs, QWidget* w);

private:
	struct Problem {
		QString name;
		QString message;
	};

	class ProblemsDialog;
};

#endif // GEODATARIVERSURVEYJMKEXPORTER_H
