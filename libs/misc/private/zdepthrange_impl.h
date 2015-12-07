#ifndef ZDEPTHRANGE_IMPL_H
#define ZDEPTHRANGE_IMPL_H

#include "../zdepthrange.h"

class ZDepthRange::Impl
{
public:
	double m_min {0};
	double m_max {1};
	unsigned int m_itemCount {0};
};

#endif // ZDEPTHRANGE_IMPL_H
