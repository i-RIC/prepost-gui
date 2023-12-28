#include "colormaplegendsettingcontainer.h"
#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapsettingcontainer.h"
#include "private/colormaplegendsettingcontainer_imagebuilder.h"

#include <QFont>

ColorMapLegendSettingContainer::ColorMapLegendSettingContainer() :
	CompositeContainer({&visible, &direction, &barAutoWidth, &barWidth, &barAlign,
										 &title, &autoNumberOfLabels, &numberOfLabels, &labelSkipRate, &labelFormat, &titleFont, &labelFont,
										 &titleColor, &labelColor, &backgroundColor, &backgroundOpacity, &showBorder, &imageSetting}),
	visible {"visible", true},
	direction {"direction", Direction::Vertical},
	barAutoWidth {"barAutoWidth", false},
	barWidth {"barWidth", 30},
	barAlign {"barAlign", BarAlign::Center},
	title {"title"},
	autoNumberOfLabels {"autoNumberOfLabels", true},
	numberOfLabels {"numberOfLabels"},
	labelSkipRate {"labelSkipRate", 1},
	labelFormat {"labelFormat", "%5.2f"},
	titleFont {"titleFont", QFont("MS UI Gothic", 14)},
	labelFont {"labelFont", QFont("MS UI Gothic", 11)},
	titleColor {"titleColor", Qt::black},
	labelColor {"labelColor", Qt::black},
	backgroundColor {"backgroundColor", Qt::white},
	backgroundOpacity {"backgroundOpacity", 20},
	showBorder {"showBorder", true},
	imageSetting {},
	m_colorMapSetting {nullptr},
	m_delegateMode {false},
	m_imageBuilder {new ImageBuilder {this}}
{
	auto& is = imageSetting;
	is.setImageBuilder(m_imageBuilder);

	is.position = ImageSettingContainer::Position::BottomRight;
	is.horizontalMargin = 0.01;
	is.verticalMargin = 0.15;
	is.width = 300;
	is.height = 200;
}

ColorMapLegendSettingContainer::ColorMapLegendSettingContainer(const ColorMapLegendSettingContainer& c) :
	ColorMapLegendSettingContainer {}
{
	copyValue(c);
}

ColorMapLegendSettingContainer::~ColorMapLegendSettingContainer()
{
	delete m_imageBuilder;
}

ColorMapSettingContainer* ColorMapLegendSettingContainer::colorMapSetting() const
{
	return m_colorMapSetting;
}

void ColorMapLegendSettingContainer::setColorMapSetting(ColorMapSettingContainer* c)
{
	m_colorMapSetting = c;
}

bool ColorMapLegendSettingContainer::delegateMode() const
{
	return m_delegateMode;
}

void ColorMapLegendSettingContainer::setDelegateMode(bool delegateMode)
{
	m_delegateMode = delegateMode;
	if (delegateMode) {
		imageSetting.setSetting(&m_colorMapSetting->legend.imageSetting);
	} else {
		imageSetting.setSetting(&imageSetting);
	}
}

ColorMapLegendSettingContainerI* ColorMapLegendSettingContainer::copy() const
{
	auto ret = new ColorMapLegendSettingContainer();
	ret->copy(*this);
	return ret;
}

void ColorMapLegendSettingContainer::copy(const ColorMapLegendSettingContainerI& setting)
{
	try {
		copyValue(dynamic_cast<const ColorMapLegendSettingContainer&> (setting));
	} catch (std::bad_cast&) {
		const auto& setting2 = dynamic_cast<const ColorMapEnumerateLegendSettingContainer&> (setting);
		visible = setting2.visible;
		direction = static_cast<Direction> (setting2.direction.value());
		barAutoWidth = setting2.barAutoWidth;
		barWidth = setting2.barWidth;
		barAlign = static_cast<BarAlign> (setting2.barAlign.value());
		title = setting2.title;
		titleFont = setting2.titleFont;
		labelFont = setting2.labelFont;
		titleColor = setting2.titleColor;
		labelColor = setting2.labelColor;
		backgroundColor = setting2.backgroundColor;
		backgroundOpacity = setting2.backgroundOpacity;
		imageSetting = setting2.imageSetting;
	}
}

void ColorMapLegendSettingContainer::copyOtherThanTitle(const ColorMapLegendSettingContainerI& setting)
{
	auto origTitle = title.value();
	copy(setting);
	title.setValue(origTitle);
}

bool ColorMapLegendSettingContainer::getVisible() const
{
	return this->visible;
}

void ColorMapLegendSettingContainer::setVisible(bool visible)
{
	this->visible = visible;
	this->visible.setDefaultValue(visible);
}

ColorMapLegendSettingContainer::Direction ColorMapLegendSettingContainer::getDirection()
{
	return direction;
}

void ColorMapLegendSettingContainer::setDirection(Direction dir)
{
	direction = dir;
}

QString ColorMapLegendSettingContainer::getTitle() const
{
	return title;
}

void ColorMapLegendSettingContainer::setTitle(const QString& t)
{
	title = t;
}

ColorMapSettingContainerI* ColorMapLegendSettingContainer::setting() const
{
	return m_colorMapSetting;
}

void ColorMapLegendSettingContainer::setSetting(ColorMapSettingContainerI* setting)
{
	m_colorMapSetting = dynamic_cast<ColorMapSettingContainer*> (setting);
}

bool ColorMapLegendSettingContainer::getBarAutoWidth() const
{
	return barAutoWidth;
}

void ColorMapLegendSettingContainer::setBarAutoWidth(bool autoWidth)
{
	barAutoWidth = autoWidth;
}

int ColorMapLegendSettingContainer::getBarWidth() const
{
	return barWidth;
}

void ColorMapLegendSettingContainer::setBarWidth(int width)
{
	barWidth = width;
}

ColorMapLegendSettingContainer::BarAlign ColorMapLegendSettingContainer::getBarAlign() const
{
	return barAlign;
}

void ColorMapLegendSettingContainer::setBarAlign(BarAlign align)
{
	barAlign = align;
}

QFont ColorMapLegendSettingContainer::getTitleFont() const
{
	return titleFont;
}

void ColorMapLegendSettingContainer::setTitleFont(const QFont& font)
{
	titleFont = font;
}

QFont ColorMapLegendSettingContainer::getLabelFont() const
{
	return labelFont;
}

void ColorMapLegendSettingContainer::setLabelFont(const QFont& font)
{
	labelFont = font;
}

QColor ColorMapLegendSettingContainer::getTitleColor() const
{
	return titleColor;
}

void ColorMapLegendSettingContainer::setTitleColor(const QColor& color)
{
	titleColor = color;
}

QColor ColorMapLegendSettingContainer::getLabelColor() const
{
	return labelColor;
}

void ColorMapLegendSettingContainer::setLabelColor(const QColor& color)
{
	labelColor = color;
}

QColor ColorMapLegendSettingContainer::getBackgroundColor() const
{
	return backgroundColor;
}

void ColorMapLegendSettingContainer::setBackgroundColor(const QColor& color)
{
	backgroundColor = color;
}

OpacityContainer ColorMapLegendSettingContainer::getBackgroundOpacity() const
{
	return backgroundOpacity;
}

void ColorMapLegendSettingContainer::setBackgroundOpacity(const OpacityContainer& opacity)
{
	backgroundOpacity = opacity;
}

bool ColorMapLegendSettingContainer::getShowBorder() const
{
	return showBorder;
}

void ColorMapLegendSettingContainer::setShowBorder(bool show)
{
	showBorder = show;
}

ImageSettingContainer* ColorMapLegendSettingContainer::imgSetting()
{
	return &imageSetting;
}

void ColorMapLegendSettingContainer::copyValue(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const ColorMapLegendSettingContainer&>(c);
	CompositeContainer::copyValue(c2);

	m_delegateMode = c2.m_delegateMode;
}


void ColorMapLegendSettingContainer::copyWithColorMap(const ColorMapLegendSettingContainer &c)
{
	copyValue(c);
	m_colorMapSetting = c.m_colorMapSetting;
}

ColorMapLegendSettingContainer& ColorMapLegendSettingContainer::operator=(const ColorMapLegendSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ColorMapLegendSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ColorMapLegendSettingContainer&> (c));
}
