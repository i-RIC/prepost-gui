#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_SETTING_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_SETTING_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <guibase/vtktextpropertysettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/intcontainer.h>

class GridCreatingConditionCenterAndWidth::Setting : public CompositeContainer
{
public:
	Setting();
	Setting(const Setting& setting);
	~Setting();

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	IntContainer iMax;
	IntContainer jMax;
	DoubleContainer width;
	DoubleContainer length;

	vtkTextPropertySettingContainer upDownText;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_SETTING_H
