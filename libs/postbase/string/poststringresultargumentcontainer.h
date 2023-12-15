#ifndef POSTSTRINGRESULTARGUMENTCONTAINER_H
#define POSTSTRINGRESULTARGUMENTCONTAINER_H

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>

class PostStringResultArgumentContainer : public CompositeContainer
{
public:
	enum class Type {
		BaseIterative,
		GridNode,
		GridCell,
		GridIEdge,
		GridJEdge,
		GridKEdge
	};

	PostStringResultArgumentContainer();
	PostStringResultArgumentContainer(const PostStringResultArgumentContainer& s);

	PostStringResultArgumentContainer& operator=(const PostStringResultArgumentContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<Type> type;
	StringContainer name;
	StringContainer refName;

	IntContainer i;
	IntContainer j;
	IntContainer k;
	IntContainer index;

	StringContainer valueForTest;
};

#endif // POSTSTRINGRESULTARGUMENTCONTAINER_H
