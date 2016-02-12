#include "colormapcustomsettingcolor.h"

#include <QColor>

ColorMapCustomSettingColor::ColorMapCustomSettingColor() :
	CompositeContainer ({&value, &color}),
	value {"value", 0},
	color {"color", Qt::black}
{}

ColorMapCustomSettingColor::ColorMapCustomSettingColor(const ColorMapCustomSettingColor& c) :
	ColorMapCustomSettingColor()
{
	copyValue(c);
}

ColorMapCustomSettingColor& ColorMapCustomSettingColor::operator=(const ColorMapCustomSettingColor& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ColorMapCustomSettingColor::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ColorMapCustomSettingColor&> (c));
}
