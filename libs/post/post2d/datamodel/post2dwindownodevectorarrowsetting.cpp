#include "post2dwindownodevectorarrowsetting.h"

Post2dWindowNodeVectorArrowSetting::Post2dWindowNodeVectorArrowSetting() :
	ArrowSettingContainer(),
	regionMode {"regionMode", StructuredGridRegion::rmFull}
{
	addContainer(&regionMode);
}

Post2dWindowNodeVectorArrowSetting::Post2dWindowNodeVectorArrowSetting(const Post2dWindowNodeVectorArrowSetting& c) :
	Post2dWindowNodeVectorArrowSetting()
{
	copyValue(c);
}

Post2dWindowNodeVectorArrowSetting::~Post2dWindowNodeVectorArrowSetting()
{}

Post2dWindowNodeVectorArrowSetting& Post2dWindowNodeVectorArrowSetting::operator=(const Post2dWindowNodeVectorArrowSetting& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorArrowSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Post2dWindowNodeVectorArrowSetting&> (c));
}
