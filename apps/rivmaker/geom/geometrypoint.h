#ifndef GEOMETRYPOINT_H
#define GEOMETRYPOINT_H

#include "geometry.h"

#include <QString>
#include <QVector3D>

class GeometryPoint : public QVector3D, public Geometry
{
public:
	GeometryPoint();
	GeometryPoint(double x, double y, double z);
	GeometryPoint(double x, double y, double z, const QString& name);

	const QString& name()	const;
	Rect rect() const override;

private:
	QString m_name;
};

inline GeometryPoint::GeometryPoint() :
	QVector3D {}
{}

inline GeometryPoint::GeometryPoint(double x, double y, double z) :
	QVector3D {x, y, z}
{}

inline GeometryPoint::GeometryPoint(double x, double y, double z, const QString& name) :
	QVector3D {x, y, z},
	m_name {name}
{}

inline const QString& GeometryPoint::name()	const
{
	return m_name;
}

#endif // GEOMETRYPOINT_H
