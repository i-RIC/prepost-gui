#ifndef COLORMAPLEGENDSETTINGCONTAINER_H
#define COLORMAPLEGENDSETTINGCONTAINER_H

#include "../guicore_global.h"
#include "colormaplegendsettingcontaineri.h"

#include <guicore/image/imagesettingcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/qfontcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class ColorMapSettingContainer;

class GUICOREDLL_EXPORT ColorMapLegendSettingContainer : public CompositeContainer, public ColorMapLegendSettingContainerI
{
public:
	enum class VisibilityMode {
		Always,
		WhenSelected,
		Never,
	};
	enum class Direction {
		Horizontal,
		Vertical
	};

	ColorMapLegendSettingContainer();
	ColorMapLegendSettingContainer(const ColorMapLegendSettingContainer& c);
	~ColorMapLegendSettingContainer();

	ColorMapSettingContainer* colorMapSetting() const;
	void setColorMapSetting(ColorMapSettingContainer* c);

	void setTitle(const QString& title) override;
	ColorMapSettingContainerI* setting() override;
	ImageSettingContainer* imgSetting() override;

	ColorMapLegendSettingContainer& operator=(const ColorMapLegendSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<VisibilityMode> visibilityMode;
	EnumContainerT<Direction> direction;

	StringContainer title;

	BoolContainer autoNumberOfLabels;
	IntContainer numberOfLabels;

	StringContainer labelFormat;

	QFontContainer titleFont;
	QFontContainer labelFont;

	ColorContainer titleColor;
	ColorContainer labelColor;
	ColorContainer backgroundColor;
	OpacityContainer backgroundOpacity;

	ImageSettingContainer imageSetting;

private:
	ColorMapSettingContainer* m_colorMapSetting;

	class ImageBuilder;
	ImageBuilder* m_imageBuilder;
};

#endif // COLORMAPLEGENDSETTINGCONTAINER_H
