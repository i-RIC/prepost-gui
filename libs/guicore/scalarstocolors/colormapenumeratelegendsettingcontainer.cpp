#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapenumeratesettingcontainer.h"
#include "private/colormapenumeratelegendsettingcontainer_imagebuilder.h"

ColorMapEnumerateLegendSettingContainer::ColorMapEnumerateLegendSettingContainer() :
	CompositeContainer({&visible, &direction, &barAutoWidth, &barWidth, &barAlign,
										 &title, &titleFont, &labelFont,
										 &titleColor, &labelColor, &backgroundColor, &backgroundOpacity, &imageSetting}),
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
	imageSetting {},
	m_colorMapSetting {nullptr},
	m_delegateMode {false},
	m_imageBuilder {new ImageBuilder {this}}
{
	imageSetting.setImageBuilder(m_imageBuilder);

	imageSetting.position = ImageSettingContainer::Position::BottomRight;
	imageSetting.horizontalMargin = 0.05;
	imageSetting.verticalMargin = 0.15;
	imageSetting.width = 150;
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

void ColorMapEnumerateLegendSettingContainer::copy(const ColorMapLegendSettingContainerI& setting)
{
	copyValue(dynamic_cast<const ColorMapEnumerateLegendSettingContainer&> (setting));
}

void ColorMapEnumerateLegendSettingContainer::setVisible(bool visible)
{
	this->visible = visible;
	this->visible.setDefaultValue(visible);
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
