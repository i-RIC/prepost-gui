#include "arrowslegendsettingcontainer.h"
#include "private/arrowslegendsettingcontainer_imagebuilder.h"

ArrowsLegendSettingContainer::ArrowsLegendSettingContainer():
	CompositeContainer({&visible,
										 &title, &titleFont,
										 &lengthFormat, &lengthFont,
										 &titleColor, &lengthColor, &backgroundColor, &backgroundOpacity, &showBorder,
										 &imageSetting}),
	visible {"visible", true},
	title {"title"},
	titleFont {"titleFont", QFont("Arial", 11)},
	lengthFormat {"lengthFormat", "%5.2f"},
	lengthFont {"lengthFont", QFont("Arial", 11)},
	titleColor {"titleColor", Qt::black},
	lengthColor {"lengthColor", Qt::black},
	backgroundColor {"backgroundColor", Qt::white},
	backgroundOpacity {"backgroundOpacity", 20},
	showBorder {"showBorder", true},
	imageSetting {},
	m_arrowsSetting {nullptr},
	m_imageBuilder {new ImageBuilder {this}}
{
	auto& is = imageSetting;
	is.setImageBuilder(m_imageBuilder);

	is.position = ImageSettingContainer::Position::BottomRight;
	is.horizontalMargin = 0.25;
	is.verticalMargin = 0.05;
	is.width = 160;
	is.height = 100;
}

ArrowsLegendSettingContainer::ArrowsLegendSettingContainer(const ArrowsLegendSettingContainer& c) :
	ArrowsLegendSettingContainer {}
{
	copyValue(c);
}

ArrowsLegendSettingContainer::~ArrowsLegendSettingContainer()
{
	delete m_imageBuilder;
}

void ArrowsLegendSettingContainer::setArrowsSetting(ArrowsSettingContainer* c)
{
	m_arrowsSetting = c;
}

ArrowsLegendSettingContainer& ArrowsLegendSettingContainer::operator=(const ArrowsLegendSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ArrowsLegendSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ArrowsLegendSettingContainer&> (c));
}
