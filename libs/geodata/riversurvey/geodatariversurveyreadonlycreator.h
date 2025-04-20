#ifndef GEODATARIVERSURVEYREADONLYCREATOR_H
#define GEODATARIVERSURVEYREADONLYCREATOR_H

#include "gd_riversurvey_global.h"
#include "geodatariversurveycreator.h"

class GD_RIVERSURVEY_EXPORT GeoDataRiverSurveyReadOnlyCreator : public GeoDataRiverSurveyCreator
{
	Q_OBJECT

public:
	GeoDataRiverSurveyReadOnlyCreator();

	bool isCreatable() const override;
	bool isReadOnly() const override;
};

#endif // GEODATARIVERSURVEYREADONLYCREATOR_H
