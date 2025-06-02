#ifndef GEODATARIVERSURVEY_AREACALCULATOR_H
#define GEODATARIVERSURVEY_AREACALCULATOR_H

#include "../geodatariversurvey.h"

class GeoDataRiverSurvey::AreaCalculator
{
public:
	AreaCalculator(GeoDataRiverSurvey* before, GeoDataRiverSurvey* after, const QString fileName, bool statistic);
	bool calculate(QWidget* w);

private:
	void calculateEach(QWidget* w, QTextStream* s);
	void calculateEach(GeoDataRiverPathPoint* before_p, GeoDataRiverPathPoint* after_p, QTextStream* s);

	void calculateStatistic(QWidget* w, QTextStream* s);
	void calculateStatistic(GeoDataRiverPathPoint* before_p, GeoDataRiverPathPoint* after_p, QTextStream* s);

	GeoDataRiverSurvey* m_before;
	GeoDataRiverSurvey* m_after;
	QString m_fileName;

	bool m_statistic;
};

#endif // GEODATARIVERSURVEY_AREACALCULATOR_H
