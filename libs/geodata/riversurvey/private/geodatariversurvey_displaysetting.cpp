#include "geodatariversurvey_displaysetting.h"

GeoDataRiverSurvey::DisplaySetting::DisplaySetting() :
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

GeoDataRiverSurvey::DisplaySetting::DisplaySetting(const DisplaySetting& setting) :
	DisplaySetting {}
{
	copyValue(setting);
}

GeoDataRiverSurvey::DisplaySetting& GeoDataRiverSurvey::DisplaySetting::operator=(const DisplaySetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& GeoDataRiverSurvey::DisplaySetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const DisplaySetting&> (c));
}
