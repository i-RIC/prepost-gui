#include "geodatariversurveydisplaysetting.h"

#include <QColor>
#include <QSettings>

GeoDataRiverSurveyDisplaySetting::GeoDataRiverSurveyDisplaySetting() :
	CompositeContainer ({&showBackground, &showLines, &opacity, &crosssectionLinesScale, &crosssectionLinesColor,
											&showNames, &namesTextSetting}),
	showBackground {"showBackground", false},
	showLines {"showLines", false},
	opacity {},
	crosssectionLinesScale {"lineZScale", 10},
	crosssectionLinesColor {"lineColor", Qt::red},
	showNames {"showNames", true},
	namesTextSetting {}
{
	opacity = 50;
	namesTextSetting.addPrefix("names");
	namesTextSetting.fontSize = 15;
	namesTextSetting.fontSize.setDefaultValue(15);

	QSettings settings;
	namesTextSetting.loadSetting(settings, "fontsetting/riversurvey_names");
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
