#include "backgroundimageinfo_setting.h"

BackgroundImageInfo::Setting::Setting() :
	CompositeContainer({&fileName, &positionX, &positionY, &scale, &angle, &fixed}),
	fileName {"filename"},
	positionX {"positionX"},
	positionY {"positionY"},
	scale {"scale"},
	angle {"angle"},
	fixed {"fixed"}
{}

BackgroundImageInfo::Setting::Setting(const Setting& setting) :
	Setting {}
{
	operator=(setting);
}

BackgroundImageInfo::Setting::~Setting()
{}

BackgroundImageInfo::Setting& BackgroundImageInfo::Setting::operator=(const Setting& setting)
{
	return dynamic_cast<Setting&>(CompositeContainer::operator=(dynamic_cast<const XmlAttributeContainer&>(setting)));
}
