#ifndef COLORMAPCUSTOMSETTINGCOLOR_H
#define COLORMAPCUSTOMSETTINGCOLOR_H

#include "../guibase_global.h"

#include <misc/compositecontainer.h>
#include <misc/colorcontainer.h>
#include <misc/doublecontainer.h>

class GUIBASEDLL_EXPORT ColorMapCustomSettingColor : public CompositeContainer
{
public:
	ColorMapCustomSettingColor();
	ColorMapCustomSettingColor(const ColorMapCustomSettingColor& c);

	ColorMapCustomSettingColor& operator=(const ColorMapCustomSettingColor& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	DoubleContainer value;
	ColorContainer color;
};

#endif // COLORMAPCUSTOMSETTINGCOLOR_H
