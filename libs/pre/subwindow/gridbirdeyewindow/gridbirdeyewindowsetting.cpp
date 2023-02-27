#include "gridbirdeyewindowsetting.h"

GridBirdEyeWindowSetting::GridBirdEyeWindowSetting() :
	CompositeContainer({&warpTarget, &mapping, &colorTarget, &customColor, &showAxes, &axesColor, &zScale, &backgroundColor}),
	warpTarget {"target"},
	mapping {"mapping", Mapping::Value},
	colorTarget {"colorTarget"},
	customColor {"customColor", Qt::blue},
	showAxes {"showAxes", false},
	axesColor {"axesColor", Qt::black},
	zScale {"zScale", 3.0},
	backgroundColor {"axesColor", Qt::white}
{}

GridBirdEyeWindowSetting::GridBirdEyeWindowSetting(const GridBirdEyeWindowSetting& setting) :
	GridBirdEyeWindowSetting {}
{
	copyValue(setting);
}

GridBirdEyeWindowSetting& GridBirdEyeWindowSetting::operator=(const GridBirdEyeWindowSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& GridBirdEyeWindowSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const GridBirdEyeWindowSetting&>(c));
}
