#include "post2dwindowcontoursetting.h"

Post2dWindowContourSetting::Post2dWindowContourSetting() :
	ScalarSettingContainer(),
	regionMode {"regionMode", StructuredGridRegion::rmFull}
{
	addContainer(&regionMode);
	addContainer(&range);

	range.setPrefix("region");
}

Post2dWindowContourSetting::Post2dWindowContourSetting(const Post2dWindowContourSetting& s) :
	Post2dWindowContourSetting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowContourSetting::~Post2dWindowContourSetting()
{}

Post2dWindowContourSetting& Post2dWindowContourSetting::operator=(const Post2dWindowContourSetting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

XmlAttributeContainer& Post2dWindowContourSetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const Post2dWindowContourSetting&>(s));
}
