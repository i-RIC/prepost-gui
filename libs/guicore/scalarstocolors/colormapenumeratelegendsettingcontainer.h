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

	ColorMapLegendSettingContainerI* copy() const override;
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
	bool getBarAutoWidth() const override;
	void setBarAutoWidth(bool autoWidth) override;

	int getBarWidth() const override;
	void setBarWidth(int width) override;

	BarAlign getBarAlign() const override;
	void setBarAlign(BarAlign align) override;

	QFont getTitleFont() const override;
	void setTitleFont(const QFont& font) override;

	QFont getLabelFont() const override;
	void setLabelFont(const QFont& font) override;

	QColor getTitleColor() const override;
	void setTitleColor(const QColor& color) override;

	QColor getLabelColor() const override;
	void setLabelColor(const QColor& color) override;

	QColor getBackgroundColor() const override;
	void setBackgroundColor(const QColor& color) override;

	OpacityContainer getBackgroundOpacity() const override;
	void setBackgroundOpacity(const OpacityContainer& opacity) override;

	bool getShowBorder() const override;
	void setShowBorder(bool show) override;

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
	BoolContainer showBorder;

	ImageSettingContainer imageSetting;

private:
	ColorMapEnumerateSettingContainer* m_colorMapSetting;
	bool m_delegateMode;

	class ImageBuilder;
	ImageBuilder* m_imageBuilder;
};

#endif // COLORMAPENUMERATELEGENDSETTINGCONTAINER_H
