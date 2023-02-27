#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapenumeratesettingcontainer.h"
#include "private/colormapenumeratelegendsettingcontainer_imagebuilder.h"

ColorMapEnumerateLegendSettingContainer::ColorMapEnumerateLegendSettingContainer() :
	CompositeContainer({&visibilityMode, &title, &titleFont, &labelFont,
										 &titleColor, &labelColor, &backgroundColor, &backgroundOpacity, &imageSetting}),
	visibilityMode {"visibilityMode", VisibilityMode::WhenSelected},
	title {"title"},
	titleFont {"titleFont", QFont("MS UI Gothic", 14)},
	labelFont {"labelFont", QFont("MS UI Gothic", 11)},
	titleColor {"titleColor", Qt::black},
	labelColor {"labelColor", Qt::black},
	backgroundColor {"backgroundColor", Qt::white},
	backgroundOpacity {"backgroundOpacity", 20},
	imageSetting {},
	m_colorMapSetting {nullptr},
	m_imageBuilder {new ImageBuilder {this}}
{
	imageSetting.setImageBuilder(m_imageBuilder);

	imageSetting.position = ImageSettingContainer::Position::BottomRight;
	imageSetting.horizontalMargin = 10;
	imageSetting.verticalMargin = 100;
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

void ColorMapEnumerateLegendSettingContainer::setTitle(const QString& t)
{
	title = t;
}

ColorMapSettingContainerI* ColorMapEnumerateLegendSettingContainer::setting()
{
	return m_colorMapSetting;
}

ImageSettingContainer* ColorMapEnumerateLegendSettingContainer::imgSetting()
{
	return &imageSetting;
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
