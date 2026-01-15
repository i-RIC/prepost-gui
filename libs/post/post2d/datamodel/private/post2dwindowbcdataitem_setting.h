#ifndef POST2DWINDOWBCDATAITEM_SETTING_H
#define POST2DWINDOWBCDATAITEM_SETTING_H

#include "../post2dwindowbcdataitem.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/colorcontainer.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>

class Post2dWindowBCDataItem::Setting : public CompositeContainer
{
public:
	Setting();
	Setting(const Setting& setting);

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;


	OpacityContainer opacity;
	ColorContainer color;
	BoolContainer showName;
	IntContainer pointSize;
};

#endif // POST2DWINDOWBCDATAITEM_SETTING_H
