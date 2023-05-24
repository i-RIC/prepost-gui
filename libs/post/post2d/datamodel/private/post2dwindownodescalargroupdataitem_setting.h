#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_SETTING_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_SETTING_H

#include "../post2dwindownodescalargroupdataitem.h"

#include <guicore/contour/contoursettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>

class Post2dWindowNodeScalarGroupDataItem::Setting : public CompositeContainer
{
public:
	Setting();
	Setting(const Setting& setting);

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	ColorMapSettingContainer colorMapSetting;
	Region2dSettingContainer regionSetting;
	ContourSettingContainer contourSetting;
	OpacityContainer opacity;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_SETTING_H