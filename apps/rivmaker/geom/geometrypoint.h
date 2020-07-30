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

	double error() const;
	void setError(double error);

private:
	QString m_name;
	double m_error;
};

inline GeometryPoint::GeometryPoint() :
	QVector3D {},
	m_error {0}
{}

inline GeometryPoint::GeometryPoint(double x, double y, double z) :
	QVector3D {x, y, z},
	m_error {0}
{}

inline GeometryPoint::GeometryPoint(double x, double y, double z, const QString& name) :
	QVector3D {x, y, z},
	m_name {name},
	m_error {0}
{}

inline const QString& GeometryPoint::name()	const
{
	return m_name;
}

inline double GeometryPoint::error() const
{
	return m_error;
}

inline void GeometryPoint::setError(double error)
{
	m_error = error;
}

#endif // GEOMETRYPOINT_H
