#ifndef GEOMETRYTRIANGLE_H
#define GEOMETRYTRIANGLE_H

#include "geometry.h"

#include <vector>

class GeometryPoint;

class GeometryTriangle : public Geometry
{
public:
	GeometryTriangle(GeometryPoint* p1, GeometryPoint* p2, GeometryPoint* p3);

	GeometryPoint* point(int idx) const;
	std::vector<GeometryPoint> crossedPoints(const QPointF& p1, const QPointF& p2) const;

	Rect rect() const override;

private:
	GeometryPoint* m_points[3];
};

#endif // GEOMETRYTRIANGLE_H
