#ifndef QPOINTFCONTAINER_IMPL_H
#define QPOINTFCONTAINER_IMPL_H

#include "../doublecontainer.h"
#include "../qpointfcontainer.h"

class QPointFContainer::Impl
{
public:
	DoubleContainer m_xValue {"x"};
	DoubleContainer m_yValue {"y"};
};

#endif // QPOINTFCONTAINER_IMPL_H

