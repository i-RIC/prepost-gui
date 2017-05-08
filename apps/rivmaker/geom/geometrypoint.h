#ifndef GEOMETRYPOINT_H
#define GEOMETRYPOINT_H

#include "geometry.h"

#include <QVector3D>

class GeometryPoint : public QVector3D, public Geometry
{
public:
	GeometryPoint();
	GeometryPoint(double x, double y, double z);

	Rect rect() const override;
};

inline GeometryPoint::GeometryPoint() :
	QVector3D {}
{}

inline GeometryPoint::GeometryPoint(double x, double y, double z) :
	QVector3D {x, y, z}
{}

#endif // GEOMETRYPOINT_H
