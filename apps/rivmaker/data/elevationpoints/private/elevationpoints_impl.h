#ifndef ELEVATIONPOINTS_IMPL_H
#define ELEVATIONPOINTS_IMPL_H

#include "../elevationpoints.h"
#include "../../../geom/geometrytriangle.h"

#include <geos/index/quadtree/Quadtree.h>

#include <vector>

class ElevationPoints::Impl
{
public:
	Impl(ElevationPoints* points);

	void buildTriangles();

	std::vector<GeometryTriangle> m_triangles;

private:
	ElevationPoints* m_parent;
};

#endif // ELEVATIONPOINTS_IMPL_H
