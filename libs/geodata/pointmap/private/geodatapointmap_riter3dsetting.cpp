#include "geodatapointmap_riter3dsetting.h"

GeoDataPointmap::Riter3dSetting::Riter3dSetting() :
	CompositeContainer({&enabled, &fileName}),
	enabled {"enabled", false},
	fileName {"fileName", ""}
{
	addPrefix("riter3d");
}

GeoDataPointmap::Riter3dSetting::Riter3dSetting(const Riter3dSetting& s) :
	Riter3dSetting {}
{
	copyValue(s);
}

GeoDataPointmap::Riter3dSetting& GeoDataPointmap::Riter3dSetting::operator=(const Riter3dSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& GeoDataPointmap::Riter3dSetting::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const Riter3dSetting&> (setting));
}
