#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapenumeratesettingcontainer.h"
#include "colormaplegendsettingcontainer.h"
#include "private/colormapenumeratelegendsettingcontainer_imagebuilder.h"

ColorMapEnumerateLegendSettingContainer::ColorMapEnumerateLegendSettingContainer() :
	CompositeContainer({&visible, &direction, &barAutoWidth, &barWidth, &barAlign,
										 &title, &titleFont, &labelFont,
										 &titleColor, &labelColor, &backgroundColor, &backgroundOpacity, &showBorder, &imageSetting}),
	visible {"visible", true},
	direction {"direction", Direction::Vertical},
	barAutoWidth {"barAutoWidth", false},
	barWidth {"barWidth", 30},
	barAlign {"barAlign", BarAlign::Center},
	title {"title"},
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
	imageSetting.setImageBuilder(m_imageBuilder);

	imageSetting.position = ImageSettingContainer::Position::BottomRight;
	imageSetting.horizontalMargin = 0.01;
	imageSetting.verticalMargin = 0.15;
	imageSetting.width = 300;
	imageSetting.height = 200;
}

ColorMapEnumerateLegendSettingContainer::ColorMapEnumerateLegendSettingContainer(const ColorMapEnumerateLegendSettingContainer& c) :
	ColorMapEnumerateLegendSettingContainer {}
{
	copyValue(c);
}

ColorMapEnumerateLegendSettingContainer::~ColorMapEnumerateLegendSettingContainer()
{
	delete m_imageBuilder;
}

ColorMapEnumerateSettingContainer* ColorMapEnumerateLegendSettingContainer::colorMapSetting() const
{
	return m_colorMapSetting;
}

void ColorMapEnumerateLegendSettingContainer::setColorMapSetting(ColorMapEnumerateSettingContainer* c)
{
	m_colorMapSetting = c;
}

bool ColorMapEnumerateLegendSettingContainer::delegateMode() const
{
	return m_delegateMode;
}

void ColorMapEnumerateLegendSettingContainer::setDelegateMode(bool delegateMode)
{
	m_delegateMode = delegateMode;
	if (delegateMode) {
		imageSetting.setSetting(&m_colorMapSetting->legend.imageSetting);
	} else {
		imageSetting.setSetting(&imageSetting);
	}
}

ColorMapLegendSettingContainerI* ColorMapEnumerateLegendSettingContainer::copy() const
{
	auto ret = new ColorMapEnumerateLegendSettingContainer();
	ret->copy(*this);
	return ret;
}

void ColorMapEnumerateLegendSettingContainer::copy(const ColorMapLegendSettingContainerI& setting)
{
	try {
		copyValue(dynamic_cast<const ColorMapEnumerateLegendSettingContainer&> (setting));
	} catch (std::bad_cast&) {
		const auto& setting2 = dynamic_cast<const ColorMapLegendSettingContainer&> (setting);
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

void ColorMapEnumerateLegendSettingContainer::copyOtherThanTitle(const ColorMapLegendSettingContainerI& setting)
{
	auto origTitle = title.value();
	copy(setting);
	title.setValue(origTitle);
}

bool ColorMapEnumerateLegendSettingContainer::getVisible() const
{
	return visible;
}

void ColorMapEnumerateLegendSettingContainer::setVisible(bool visible)
{
	this->visible = visible;
	this->visible.setDefaultValue(visible);
}

ColorMapEnumerateLegendSettingContainer::Direction ColorMapEnumerateLegendSettingContainer::getDirection()
{
	return direction;
}

void ColorMapEnumerateLegendSettingContainer::setDirection(Direction dir)
{
	direction = dir;
}

QString ColorMapEnumerateLegendSettingContainer::getTitle() const
{
	return title;
}

void ColorMapEnumerateLegendSettingContainer::setTitle(const QString& t)
{
	title = t;
}

ColorMapSettingContainerI* ColorMapEnumerateLegendSettingContainer::setting() const
{
	return m_colorMapSetting;
}

void ColorMapEnumerateLegendSettingContainer::setSetting(ColorMapSettingContainerI* setting)
{
	m_colorMapSetting = dynamic_cast<ColorMapEnumerateSettingContainer*> (setting);
}

bool ColorMapEnumerateLegendSettingContainer::getBarAutoWidth() const
{
	return barAutoWidth;
}

void ColorMapEnumerateLegendSettingContainer::setBarAutoWidth(bool autoWidth)
{
	barAutoWidth = autoWidth;
}

int ColorMapEnumerateLegendSettingContainer::getBarWidth() const
{
	return barWidth;
}

void ColorMapEnumerateLegendSettingContainer::setBarWidth(int width)
{
	barWidth = width;
}

ColorMapEnumerateLegendSettingContainer::BarAlign ColorMapEnumerateLegendSettingContainer::getBarAlign() const
{
	return barAlign;
}

void ColorMapEnumerateLegendSettingContainer::setBarAlign(BarAlign align)
{
	barAlign = align;
}

QFont ColorMapEnumerateLegendSettingContainer::getTitleFont() const
{
	return titleFont;
}

void ColorMapEnumerateLegendSettingContainer::setTitleFont(const QFont& font)
{
	titleFont = font;
}

QFont ColorMapEnumerateLegendSettingContainer::getLabelFont() const
{
	return labelFont;
}

void ColorMapEnumerateLegendSettingContainer::setLabelFont(const QFont& font)
{
	labelFont = font;
}

QColor ColorMapEnumerateLegendSettingContainer::getTitleColor() const
{
	return titleColor;
}

void ColorMapEnumerateLegendSettingContainer::setTitleColor(const QColor& color)
{
	titleColor = color;
}

QColor ColorMapEnumerateLegendSettingContainer::getLabelColor() const
{
	return labelColor;
}

void ColorMapEnumerateLegendSettingContainer::setLabelColor(const QColor& color)
{
	labelColor = color;
}

QColor ColorMapEnumerateLegendSettingContainer::getBackgroundColor() const
{
	return backgroundColor;
}

void ColorMapEnumerateLegendSettingContainer::setBackgroundColor(const QColor& color)
{
	backgroundColor = color;
}

OpacityContainer ColorMapEnumerateLegendSettingContainer::getBackgroundOpacity() const
{
	return backgroundOpacity;
}

void ColorMapEnumerateLegendSettingContainer::setBackgroundOpacity(const OpacityContainer& opacity)
{
	backgroundOpacity = opacity;
}

bool ColorMapEnumerateLegendSettingContainer::getShowBorder() const
{
	return showBorder;
}

void ColorMapEnumerateLegendSettingContainer::setShowBorder(bool show)
{
	showBorder = show;
}

ImageSettingContainer* ColorMapEnumerateLegendSettingContainer::imgSetting()
{
	return &imageSetting;
}

void ColorMapEnumerateLegendSettingContainer::copyValue(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const ColorMapEnumerateLegendSettingContainer&>(c);
	CompositeContainer::copyValue(c2);

	m_delegateMode = c2.m_delegateMode;
}

void ColorMapEnumerateLegendSettingContainer::copyWithColorMap(const ColorMapEnumerateLegendSettingContainer &c)
{
	copyValue(c);
	m_colorMapSetting = c.m_colorMapSetting;
}

ColorMapEnumerateLegendSettingContainer& ColorMapEnumerateLegendSettingContainer::operator=(const ColorMapEnumerateLegendSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ColorMapEnumerateLegendSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ColorMapEnumerateLegendSettingContainer&>(c));
}
