#ifndef POST2DBIRDEYEWINDOWCONTOURSETTING_H
#define POST2DBIRDEYEWINDOWCONTOURSETTING_H

#include "../post2dbirdeye_global.h"

#include <guibase/structuredgridregion.h>
#include <guibase/scalarsettingcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

class POST2DBIRDEYE_EXPORT Post2dBirdEyeWindowContourSetting : public ScalarSettingContainer
{

public:
	enum class ColorMode {
		Custom,
		ByScalar
	};

	Post2dBirdEyeWindowContourSetting();
	Post2dBirdEyeWindowContourSetting(const Post2dBirdEyeWindowContourSetting& s);
	~Post2dBirdEyeWindowContourSetting();

	Post2dBirdEyeWindowContourSetting& operator=(const Post2dBirdEyeWindowContourSetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s) override;

	StringContainer elevationTarget;

	// Region setting
	EnumContainerT<StructuredGridRegion::RegionMode> regionMode;
	StructuredGridRegion::Range2d range;

	// Color mode setting
	EnumContainerT<ColorMode> colorMode;
	ColorContainer customColor;
};


#endif // POST2DBIRDEYEWINDOWCONTOURSETTING_H
