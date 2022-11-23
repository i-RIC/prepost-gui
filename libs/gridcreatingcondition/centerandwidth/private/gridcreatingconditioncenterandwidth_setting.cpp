#include "gridcreatingconditioncenterandwidth_setting.h"

#include <QSettings>

GridCreatingConditionCenterAndWidth::Setting::Setting() :
	CompositeContainer({&iMax, &jMax, &width, &length, &upDownText}),
	iMax {"imax", 11},
	jMax {"jmax", 11},
	width {"width", 10.0},
	length {"length", 0},
	upDownText {}
{
	upDownText.addPrefix("upstream");
	upDownText.fontSize = 17;
	upDownText.fontSize.setDefaultValue(17);
	upDownText.isBold = true;
	upDownText.isBold.setDefaultValue(true);

	QSettings settings;
	upDownText.loadSetting(settings, "fontsetting/upstream");
}

GridCreatingConditionCenterAndWidth::Setting::Setting(const Setting& setting) :
	Setting {}
{
	CompositeContainer::copyValue(setting);
}

GridCreatingConditionCenterAndWidth::Setting::~Setting()
{}

GridCreatingConditionCenterAndWidth::Setting& GridCreatingConditionCenterAndWidth::Setting::operator=(const Setting& setting)
{
	CompositeContainer::copyValue(setting);
	return *this;
}

XmlAttributeContainer& GridCreatingConditionCenterAndWidth::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator =(dynamic_cast<const Setting&>(c));
}
