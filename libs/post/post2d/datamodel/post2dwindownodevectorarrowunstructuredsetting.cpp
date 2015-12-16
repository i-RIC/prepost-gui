#include "post2dwindownodevectorarrowunstructuredsetting.h"

Post2dWindowNodeVectorArrowUnstructuredSetting::Post2dWindowNodeVectorArrowUnstructuredSetting() :
	Post2dWindowNodeVectorArrowSetting()
{}

Post2dWindowNodeVectorArrowUnstructuredSetting::Post2dWindowNodeVectorArrowUnstructuredSetting(const Post2dWindowNodeVectorArrowUnstructuredSetting& c) :
	Post2dWindowNodeVectorArrowUnstructuredSetting()
{
	CompositeContainer::copyValue(c);
}

Post2dWindowNodeVectorArrowUnstructuredSetting& Post2dWindowNodeVectorArrowUnstructuredSetting::operator=(const Post2dWindowNodeVectorArrowUnstructuredSetting& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeVectorArrowUnstructuredSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Post2dWindowNodeVectorArrowUnstructuredSetting&> (c));
}
