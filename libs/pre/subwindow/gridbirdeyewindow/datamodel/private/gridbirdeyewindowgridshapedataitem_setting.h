#ifndef GRIDBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTING_H
#define GRIDBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTING_H

#include "../gridbirdeyewindowgridshapedataitem.h"

#include <guibase/gridshape/gridshapesettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

class GridBirdEyeWindowGridShapeDataItem::Setting : public CompositeContainer
{
public:
	Setting();
	Setting(const Setting& setting);

	Setting& operator=(const Setting& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	StringContainer elevationTarget;
	GridShapeSettingContainer gridShape;
};

#endif // GRIDBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTING_H
