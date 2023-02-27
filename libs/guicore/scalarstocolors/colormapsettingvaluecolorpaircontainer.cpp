#include "colormapsettingvaluecolorpaircontainer.h"

#include <QColor>

ColorMapSettingValueColorPairContainer::ColorMapSettingValueColorPairContainer() :
	CompositeContainer ({&value, &color, &transparent}),
	value {"value", 0},
	color {"color", Qt::black},
	transparent {"transparent", false}
{}

ColorMapSettingValueColorPairContainer::ColorMapSettingValueColorPairContainer(const ColorMapSettingValueColorPairContainer& c) :
	ColorMapSettingValueColorPairContainer()
{
	copyValue(c);
}

ColorMapSettingValueColorPairContainer::ColorMapSettingValueColorPairContainer(double v, const QColor& c) :
	ColorMapSettingValueColorPairContainer {}
{
	value = v;
	color = c;
}

ColorMapSettingValueColorPairContainer& ColorMapSettingValueColorPairContainer::operator=(const ColorMapSettingValueColorPairContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ColorMapSettingValueColorPairContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ColorMapSettingValueColorPairContainer&> (c));
}
