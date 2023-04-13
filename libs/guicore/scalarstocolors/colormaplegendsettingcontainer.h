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
	enum class Direction {
		Horizontal,
		Vertical
	};
	enum class BarAlign {
		Left,
		Center,
		Right
	};

	ColorMapLegendSettingContainer();
	ColorMapLegendSettingContainer(const ColorMapLegendSettingContainer& c);
	~ColorMapLegendSettingContainer();

	ColorMapSettingContainer* colorMapSetting() const;
	void setColorMapSetting(ColorMapSettingContainer* c);

	bool delegateMode() const override;
	void setDelegateMode(bool delegateMode) override;

	void copy(const ColorMapLegendSettingContainerI& setting) override;
	void setVisible(bool visible) override;
	void setTitle(const QString& title) override;
	ColorMapSettingContainerI* setting() const override;
	void setSetting(ColorMapSettingContainerI* setting) override;
	ImageSettingContainer* imgSetting() override;

	void copyValue(const XmlAttributeContainer& c) override;
	void copyWithColorMap(const ColorMapLegendSettingContainer& c);
	ColorMapLegendSettingContainer& operator=(const ColorMapLegendSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	BoolContainer visible;
	EnumContainerT<Direction> direction;
	BoolContainer barAutoWidth;
	IntContainer barWidth;
	EnumContainerT<BarAlign> barAlign;

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
	bool m_delegateMode;

	class ImageBuilder;
	ImageBuilder* m_imageBuilder;
};

#endif // COLORMAPLEGENDSETTINGCONTAINER_H
