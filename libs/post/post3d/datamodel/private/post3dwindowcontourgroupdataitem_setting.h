#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_SETTING_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_SETTING_H

#include "../post3dwindowcontourgroupdataitem.h"

#include <guicore/contour/contoursettingcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>

class ColorMapSettingContainerI;

class Post3dWindowContourGroupDataItem::Setting : public CompositeContainer
{
public:
	Setting();
	Setting(const Setting& s);

	Setting& operator=(const Setting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	ColorMapSettingContainerI* colorMapSetting;
	ContourSettingContainer contourSetting;
	BoolContainer lighting;
	OpacityContainer opacity;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_SETTING_H
