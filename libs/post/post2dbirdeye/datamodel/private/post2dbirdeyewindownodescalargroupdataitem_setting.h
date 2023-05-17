#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETTING_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETTING_H

#include "../post2dbirdeyewindownodescalargroupdataitem.h"

#include <guicore/contour/contoursettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting : public CompositeContainer {
public:
	enum class ColorMode {
		Custom,
		ByNodeScalar,
		ByCellScalar,
	};

	Setting();
	Setting(const Setting& setting);

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	Region2dSettingContainer regionSetting;

	EnumContainerT<ColorMode> colorMode;
	ColorContainer customColor;
	StringContainer colorTarget;
	ContourSettingContainer contourSetting;

	OpacityContainer opacity;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETTING_H
