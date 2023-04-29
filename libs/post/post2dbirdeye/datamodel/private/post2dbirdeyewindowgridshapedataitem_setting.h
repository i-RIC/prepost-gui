#ifndef POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTING_H
#define POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTING_H

#include "../post2dbirdeyewindowgridshapedataitem.h"

#include <guibase/gridshape/gridshapesettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

class Post2dBirdEyeWindowGridShapeDataItem::Setting : public CompositeContainer
{
public:
	Setting();
	Setting(const Setting& setting);

	Setting& operator=(const Setting& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	StringContainer elevationTarget;
	GridShapeSettingContainer gridShape;
};

#endif // POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTING_H
