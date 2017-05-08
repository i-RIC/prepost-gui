#ifndef ELEVATIONPOINTS_IMPL_H
#define ELEVATIONPOINTS_IMPL_H

#include "../elevationpoints.h"
#include "../../../geom/geometrytriangle.h"
#include "../../../geom/quadtreecontainer.h"

#include <vector>

class ElevationPoints::Impl
{
public:
	Impl(ElevationPoints* points);

	void buildTriangles();
	void buildContainers();

	std::vector<GeometryTriangle> m_triangles;

	QuadtreeContainer m_pointsContainer;
	QuadtreeContainer m_trianglesContainer;

private:
	ElevationPoints* m_parent;
};

#endif // ELEVATIONPOINTS_IMPL_H
