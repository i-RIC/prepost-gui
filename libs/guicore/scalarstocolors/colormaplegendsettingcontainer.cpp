#include "colormaplegendsettingcontainer.h"
#include "colormapsettingcontainer.h"
#include "private/colormaplegendsettingcontainer_imagebuilder.h"

#include <QFont>

ColorMapLegendSettingContainer::ColorMapLegendSettingContainer() :
	CompositeContainer({&visibilityMode, &title, &numberOfLabels, &labelFormat, &titleFont, &labelFont,
										 &titleColor, &labelColor, &backgroundColor, &backgroundOpacity, &imageSetting}),
	visibilityMode {"visibilityMode", VisibilityMode::WhenSelected},
	title {"title"},
	numberOfLabels {"numberOfLabels"},
	labelFormat {"labelFormat", "%5.2f"},
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
	auto& is = imageSetting;
	is.setImageBuilder(m_imageBuilder);

	is.position = ImageSettingContainer::Position::BottomRight;
	is.horizontalMargin = 10;
	is.verticalMargin = 100;
	is.width = 150;
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

void ColorMapLegendSettingContainer::setTitle(const QString& t)
{
	title = t;
}

ColorMapSettingContainerI* ColorMapLegendSettingContainer::setting()
{
	return m_colorMapSetting;
}

ImageSettingContainer* ColorMapLegendSettingContainer::imgSetting()
{
	return &imageSetting;
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
