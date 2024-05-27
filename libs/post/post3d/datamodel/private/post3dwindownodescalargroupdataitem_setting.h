#ifndef POST3DWINDOWNODESCALARGROUPDATAITEM_SETTING_H
#define POST3DWINDOWNODESCALARGROUPDATAITEM_SETTING_H

#include "../post3dwindownodescalargroupdataitem.h"

#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class Post3dWindowNodeScalarGroupDataItem::Setting : public CompositeContainer
{
public:
	Setting();
	Setting(const Setting& setting);

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	StringContainer target;
	BoolContainer fullRange;
	StructuredGridRegion::Range3d range;
	DoubleContainer isoValue;
	ColorContainer color;
	OpacityContainer opacity;
};

#endif // POST3DWINDOWNODESCALARGROUPDATAITEM_SETTING_H
