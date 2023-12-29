#ifndef COLORMAPLEGENDSETTINGCONTAINERI_H
#define COLORMAPLEGENDSETTINGCONTAINERI_H

class ColorMapSettingContainerI;
class ImageSettingContainer;
class OpacityContainer;

class ColorMapLegendSettingContainerI
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

	virtual ~ColorMapLegendSettingContainerI() {};

	virtual ColorMapLegendSettingContainerI* copy() const = 0;
	virtual void copy(const ColorMapLegendSettingContainerI& setting) = 0;
	virtual void copyOtherThanTitle(const ColorMapLegendSettingContainerI& setting) = 0;

	virtual bool getVisible() const = 0;
	virtual void setVisible(bool visible) = 0;

	virtual Direction getDirection() = 0;
	virtual void setDirection(Direction direction) = 0;

	virtual QString getTitle() const = 0;
	virtual void setTitle(const QString& title) = 0;

	virtual ColorMapSettingContainerI* setting() const = 0;
	virtual void setSetting(ColorMapSettingContainerI* setting) = 0;

	virtual bool getBarAutoWidth() const = 0;
	virtual void setBarAutoWidth(bool autoWidth) = 0;

	virtual int getBarWidth() const = 0;
	virtual void setBarWidth(int width) = 0;

	virtual BarAlign getBarAlign() const = 0;
	virtual void setBarAlign(BarAlign align) = 0;

	virtual QFont getTitleFont() const = 0;
	virtual void setTitleFont(const QFont& font) = 0;

	virtual QFont getLabelFont() const = 0;
	virtual void setLabelFont(const QFont& font) = 0;

	virtual QColor getTitleColor() const = 0;
	virtual void setTitleColor(const QColor& color) = 0;

	virtual QColor getLabelColor() const = 0;
	virtual void setLabelColor(const QColor& color) = 0;

	virtual QColor getBackgroundColor() const = 0;
	virtual void setBackgroundColor(const QColor& color) = 0;

	virtual OpacityContainer getBackgroundOpacity() const = 0;
	virtual void setBackgroundOpacity(const OpacityContainer& opacity) = 0;

	virtual bool getShowBorder() const = 0;
	virtual void setShowBorder(bool show) = 0;

	virtual ImageSettingContainer* imgSetting() = 0;
	virtual bool delegateMode() const = 0;
	virtual void setDelegateMode(bool delegateMode) = 0;
};

#endif // COLORMAPLEGENDSETTINGCONTAINERI_H
