#include "post2dwindownodevectorarrowstructuredsetting.h"

Post2dWindowNodeVectorArrowStructuredSetting::Post2dWindowNodeVectorArrowStructuredSetting() :
	Post2dWindowNodeVectorArrowSetting(),
	iSampleRate {"iSampleRate", 1},
	jSampleRate {"jSampleRate", 1},
	range {}
{
	addContainer(&iSampleRate);
	addContainer(&jSampleRate);
	addContainer(&range);

	range.addPrefix("region");
}

Post2dWindowNodeVectorArrowStructuredSetting::Post2dWindowNodeVectorArrowStructuredSetting(const Post2dWindowNodeVectorArrowStructuredSetting &c) :
	Post2dWindowNodeVectorArrowStructuredSetting()
{
	CompositeContainer::copyValue(c);
}

Post2dWindowNodeVectorArrowStructuredSetting& Post2dWindowNodeVectorArrowStructuredSetting::operator=(const Post2dWindowNodeVectorArrowStructuredSetting& c)
{
	CompositeContainer::copyValue(c);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorArrowStructuredSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Post2dWindowNodeVectorArrowStructuredSetting&> (c));
}
