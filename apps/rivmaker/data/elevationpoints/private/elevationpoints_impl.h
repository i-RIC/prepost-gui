#ifndef ELEVATIONPOINTS_IMPL_H
#define ELEVATIONPOINTS_IMPL_H

#include "../elevationpoints.h"
#include "pointstriangle.h"

#include <vector>

class ElevationPoints::Impl
{
public:
	Impl(ElevationPoints* points);

	void buildTriangles();

	std::vector<PointsTriangle> m_triangles;

private:
	ElevationPoints* m_parent;
};

#endif // ELEVATIONPOINTS_IMPL_H
