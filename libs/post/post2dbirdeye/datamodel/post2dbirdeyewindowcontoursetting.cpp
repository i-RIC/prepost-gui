#include "post2dbirdeyewindowcontoursetting.h"

Post2dBirdEyeWindowContourSetting::Post2dBirdEyeWindowContourSetting() :
	ScalarSettingContainer(),
	elevationTarget {"elevationTarget"},
	regionMode {"regionMode", StructuredGridRegion::rmFull},
	colorMode {"colorMode", ColorMode::ByScalar},
	customColor {"customColor", Qt::black}
{
	addContainer(&elevationTarget);
	addContainer(&regionMode);
	addContainer(&range);
	addContainer(&colorMode);
	addContainer(&customColor);

	range.addPrefix("region");
}

Post2dBirdEyeWindowContourSetting::Post2dBirdEyeWindowContourSetting(const Post2dBirdEyeWindowContourSetting& s) :
	Post2dBirdEyeWindowContourSetting()
{
	CompositeContainer::copyValue(s);
}

Post2dBirdEyeWindowContourSetting::~Post2dBirdEyeWindowContourSetting()
{}

Post2dBirdEyeWindowContourSetting& Post2dBirdEyeWindowContourSetting::operator=(const Post2dBirdEyeWindowContourSetting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

XmlAttributeContainer& Post2dBirdEyeWindowContourSetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const Post2dBirdEyeWindowContourSetting&>(s));
}
