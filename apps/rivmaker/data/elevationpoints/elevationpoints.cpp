#include "elevationpoints.h"

#include <QVector3D>

ElevationPoints::ElevationPoints()
{}

ElevationPoints::~ElevationPoints()
{
	for (auto v : m_points) {
		delete v;
	}
}

const std::vector<QVector3D*>& ElevationPoints::points() const
{
	return m_points;
}

std::vector<QVector3D*>& ElevationPoints::points()
{
	return m_points;
}
