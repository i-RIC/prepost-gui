#include "geodatariversurveydisplaysetting.h"

GeoDataRiverSurveyDisplaySetting::GeoDataRiverSurveyDisplaySetting() :
	CompositeContainer ({&showBackground, &showLines, &opacity, &crosssectionLinesScale, &crosssectionLinesColor}) ,
	showBackground {"showBackground", false},
	showLines {"showLines", false},
	opacity {},
	crosssectionLinesScale {"lineZScale", 10},
	crosssectionLinesColor {"lineColor", Qt::red}
{
	opacity = 50;
}

GeoDataRiverSurveyDisplaySetting::GeoDataRiverSurveyDisplaySetting(const GeoDataRiverSurveyDisplaySetting& setting) :
	GeoDataRiverSurveyDisplaySetting()
{
	copyValue(setting);
}

GeoDataRiverSurveyDisplaySetting& GeoDataRiverSurveyDisplaySetting::operator=(const GeoDataRiverSurveyDisplaySetting& setting)
{
	copyValue(setting);
	return *this;
}
