#ifndef COLORMAPSETTINGVALUECOLORPAIRCONTAINER_H
#define COLORMAPSETTINGVALUECOLORPAIRCONTAINER_H

#include "../guicore_global.h"

#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>

class GUICOREDLL_EXPORT ColorMapSettingValueColorPairContainer : public CompositeContainer
{
public:
	ColorMapSettingValueColorPairContainer();
	ColorMapSettingValueColorPairContainer(const ColorMapSettingValueColorPairContainer& c);
	ColorMapSettingValueColorPairContainer(double value, const QColor& color);

	ColorMapSettingValueColorPairContainer& operator=(const ColorMapSettingValueColorPairContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	DoubleContainer value;
	ColorContainer color;
	BoolContainer transparent;
};

#endif // COLORMAPSETTINGVALUECOLORPAIRCONTAINER_H
