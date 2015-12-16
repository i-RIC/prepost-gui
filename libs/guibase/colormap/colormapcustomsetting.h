#ifndef COLORMAPCUSTOMSETTING_H
#define COLORMAPCUSTOMSETTING_H

#include "../guibase_global.h"

#include <misc/colorcontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>

#include <vector>

#include "colormapcustomsettingcolor.h"

class GUIBASEDLL_EXPORT ColorMapCustomSetting
{
public:
	enum class Type {
		TwoColors,    ///< Color map with two colors
		ThreeColors,  ///< Color map with three colors
		Arbitrary     ///< Color map with arbitrary number colors
	};

	ColorMapCustomSetting();
	void load(const QDomNode&);
	void save(QXmlStreamWriter&) const;

	EnumContainerT<Type> type;

	ColorContainer maxColor;
	ColorContainer midColor;
	ColorContainer minColor;

	DoubleContainer midValue;

	std::vector<ColorMapCustomSettingColor> arbitrarySettings;
};

#endif // COLORMAPCUSTOMSETTING_H
