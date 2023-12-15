#include "poststringresultargumentcontainer.h"

PostStringResultArgumentContainer::PostStringResultArgumentContainer() :
	CompositeContainer({&type, &name, &refName, &i, &j, &k, &index, &valueForTest}),
	type {"type"},
	name {"name"},
	refName {"refName"},
	i {"i"},
	j {"j"},
	k {"k"},
	index {"index"},
	valueForTest {"valueForTest"}
{}

PostStringResultArgumentContainer::PostStringResultArgumentContainer(const PostStringResultArgumentContainer& s) :
	PostStringResultArgumentContainer {}
{
	copyValue(s);
}

PostStringResultArgumentContainer& PostStringResultArgumentContainer::operator=(const PostStringResultArgumentContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& PostStringResultArgumentContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const PostStringResultArgumentContainer&> (c));
}
