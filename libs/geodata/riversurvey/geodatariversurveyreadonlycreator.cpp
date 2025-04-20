#include "geodatariversurveyreadonlycreator.h"

GeoDataRiverSurveyReadOnlyCreator::GeoDataRiverSurveyReadOnlyCreator() :
	GeoDataRiverSurveyCreator {"riversurveyReadOnly"}
{}

bool GeoDataRiverSurveyReadOnlyCreator::isCreatable() const
{
	return false;
}

bool GeoDataRiverSurveyReadOnlyCreator::isReadOnly() const
{
	return true;
}
