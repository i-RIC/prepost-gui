#include "colormapcustomsettingcolor.h"

ColorMapCustomSettingColor::ColorMapCustomSettingColor() :
	CompositeContainer ({&value, &color}),
	value {"value", 0},
	color {"color", Qt::black}
{}

ColorMapCustomSettingColor::ColorMapCustomSettingColor(const ColorMapCustomSettingColor& c) :
	ColorMapCustomSettingColor()
{
	CompositeContainer::copyValue(c);
}

XmlAttributeContainer& ColorMapCustomSettingColor::operator=(const XmlAttributeContainer& c)
{
	CompositeContainer::copyValue(dynamic_cast<const ColorMapCustomSettingColor&> (c));
	return *this;
}
