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
	ColorMapLegendSettingContainer();
	ColorMapLegendSettingContainer(const ColorMapLegendSettingContainer& c);
	~ColorMapLegendSettingContainer();

	ColorMapSettingContainer* colorMapSetting() const;
	void setColorMapSetting(ColorMapSettingContainer* c);

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
	IntContainer labelSkipRate;

	StringContainer labelFormat;

	QFontContainer titleFont;
	QFontContainer labelFont;

	ColorContainer titleColor;
	ColorContainer labelColor;
	ColorContainer backgroundColor;
	OpacityContainer backgroundOpacity;
	BoolContainer showBorder;

	ImageSettingContainer imageSetting;

private:
	ColorMapSettingContainer* m_colorMapSetting;
	bool m_delegateMode;

	class ImageBuilder;
	ImageBuilder* m_imageBuilder;
};

#endif // COLORMAPLEGENDSETTINGCONTAINER_H
