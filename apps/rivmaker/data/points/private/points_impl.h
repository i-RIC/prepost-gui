#ifndef POINTS_IMPL_H
#define POINTS_IMPL_H

#include "../points.h"

class Points::Impl
{
public:
	std::vector<GeometryPoint*> m_points;
};

#endif // POINTS_IMPL_H
