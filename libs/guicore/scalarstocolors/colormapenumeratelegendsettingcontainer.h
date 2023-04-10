#ifndef COLORMAPENUMERATELEGENDSETTINGCONTAINER_H
#define COLORMAPENUMERATELEGENDSETTINGCONTAINER_H

#include "../guicore_global.h"

#include "colormaplegendsettingcontaineri.h"

#include <guicore/image/imagesettingcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/qfontcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class ColorMapEnumerateSettingContainer;

class GUICOREDLL_EXPORT ColorMapEnumerateLegendSettingContainer : public CompositeContainer, public ColorMapLegendSettingContainerI
{
public:
	enum class VisibilityMode {
		Always,
		WhenSelected,
		Never,
	};

	ColorMapEnumerateLegendSettingContainer();
	ColorMapEnumerateLegendSettingContainer(const ColorMapEnumerateLegendSettingContainer& c);
	~ColorMapEnumerateLegendSettingContainer();

	ColorMapEnumerateSettingContainer* colorMapSetting() const;
	void setColorMapSetting(ColorMapEnumerateSettingContainer* c);

	void copy(const ColorMapLegendSettingContainerI& setting) override;
	void setTitle(const QString& title) override;
	ColorMapSettingContainerI* setting() const override;
	void setSetting(ColorMapSettingContainerI* setting) override;
	ImageSettingContainer* imgSetting() override;

	ColorMapEnumerateLegendSettingContainer& operator=(const ColorMapEnumerateLegendSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<VisibilityMode> visibilityMode;

	StringContainer title;

	QFontContainer titleFont;
	QFontContainer labelFont;

	ColorContainer titleColor;
	ColorContainer labelColor;
	ColorContainer backgroundColor;
	OpacityContainer backgroundOpacity;

	ImageSettingContainer imageSetting;

private:
	ColorMapEnumerateSettingContainer* m_colorMapSetting;

	class ImageBuilder;
	ImageBuilder* m_imageBuilder;
};

#endif // COLORMAPENUMERATELEGENDSETTINGCONTAINER_H
