#include "colormaplegendsettingcontainer.h"
#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapsettingcontainer.h"
#include "private/colormaplegendsettingcontainer_imagebuilder.h"

#include <QFont>

ColorMapLegendSettingContainer::ColorMapLegendSettingContainer() :
	CompositeContainer({&visible, &direction, &barAutoWidth, &barWidth, &barAlign,
										 &title, &autoNumberOfLabels, &numberOfLabels, &labelFormat, &titleFont, &labelFont,
										 &titleColor, &labelColor, &backgroundColor, &backgroundOpacity, &showBorder, &imageSetting}),
	visible {"visible", true},
	direction {"direction", Direction::Vertical},
	barAutoWidth {"barAutoWidth", false},
	barWidth {"barWidth", 30},
	barAlign {"barAlign", BarAlign::Center},
	title {"title"},
	autoNumberOfLabels {"autoNumberOfLabels", true},
	numberOfLabels {"numberOfLabels"},
	labelFormat {"labelFormat", "%5.2f"},
	titleFont {"titleFont", QFont("Arial", 14)},
	labelFont {"labelFont", QFont("Arial", 11)},
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
