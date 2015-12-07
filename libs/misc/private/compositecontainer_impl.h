#ifndef COMPOSITECONTAINER_IMPL_H
#define COMPOSITECONTAINER_IMPL_H

#include "../compositecontainer.h"

#include <vector>

class CompositeContainer::Impl
{
public:
	std::vector<XmlAttributeContainer*> m_containers {};
};

#endif // COMPOSITECONTAINER_IMPL_H
