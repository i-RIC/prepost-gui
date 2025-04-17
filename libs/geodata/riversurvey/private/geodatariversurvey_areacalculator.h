#ifndef GEODATARIVERSURVEY_AREACALCULATOR_H
#define GEODATARIVERSURVEY_AREACALCULATOR_H

#include "../geodatariversurvey.h"

class GeoDataRiverSurvey::AreaCalculator
{
public:
	AreaCalculator(GeoDataRiverSurvey* before, GeoDataRiverSurvey* after, const QString fileName);
	bool calculate(QWidget* w);

private:
	void calculate(GeoDataRiverPathPoint* before_p, GeoDataRiverPathPoint* after_p, QTextStream* s);

	GeoDataRiverSurvey* m_before;
	GeoDataRiverSurvey* m_after;
	QString m_fileName;
};

#endif // GEODATARIVERSURVEY_AREACALCULATOR_H
