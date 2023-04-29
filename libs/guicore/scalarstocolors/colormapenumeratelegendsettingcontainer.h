#ifndef COLORMAPENUMERATELEGENDSETTINGCONTAINER_H
#define COLORMAPENUMERATELEGENDSETTINGCONTAINER_H

#include "../guicore_global.h"

#include "colormaplegendsettingcontaineri.h"

#include <guicore/image/imagesettingcontainer.h>
#include <misc/boolcontainer.h>
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
	ColorMapEnumerateLegendSettingContainer();
	ColorMapEnumerateLegendSettingContainer(const ColorMapEnumerateLegendSettingContainer& c);
	~ColorMapEnumerateLegendSettingContainer();

	ColorMapEnumerateSettingContainer* colorMapSetting() const;
	void setColorMapSetting(ColorMapEnumerateSettingContainer* c);

	bool delegateMode() const override;
	void setDelegateMode(bool delegateMode) override;

	void copy(const ColorMapLegendSettingContainerI& setting) override;
	void copyOtherThanTitle(const ColorMapLegendSettingContainerI& setting) override;
	bool getVisible() const override;
	void setVisible(bool visible) override;
	Direction getDirection() override;
	void setDirection(Direction direction) override;
	QString getTitle() const override;
	void setTitle(const QString& title) override;
	ColorMapSettingContainerI* setting() const override;
	void setSetting(ColorMapSettingContainerI* setting) override;
	ImageSettingContainer* imgSetting() override;

	void copyValue(const XmlAttributeContainer& c) override;
	void copyWithColorMap(const ColorMapEnumerateLegendSettingContainer& c);
	ColorMapEnumerateLegendSettingContainer& operator=(const ColorMapEnumerateLegendSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	BoolContainer visible;
	EnumContainerT<Direction> direction;
	BoolContainer barAutoWidth;
	IntContainer barWidth;
	EnumContainerT<BarAlign> barAlign;

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
	bool m_delegateMode;

	class ImageBuilder;
	ImageBuilder* m_imageBuilder;
};

#endif // COLORMAPENUMERATELEGENDSETTINGCONTAINER_H
