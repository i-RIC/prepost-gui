#include "watersurfaceelevationpoints.h"

#include <QVector3D>

namespace {

void clearVector(std::vector<QVector3D*>* vec)
{
	for (auto v : *vec) {
		delete v;
	}
	vec->clear();
}

} // namespace

WaterSurfaceElevationPoints::WaterSurfaceElevationPoints(DataItem *parent) :
	DataItem {parent}
{}

WaterSurfaceElevationPoints::~WaterSurfaceElevationPoints()
{
	clearVector(&m_leftBankPoints);
	clearVector(&m_rightBankPoints);
	clearVector(&m_arbitraryPoints);
}

const std::vector<QVector3D*>& WaterSurfaceElevationPoints::leftBankPoints() const
{
	return m_leftBankPoints;
}

std::vector<QVector3D*>& WaterSurfaceElevationPoints::leftBankPoints()
{
	return m_leftBankPoints;
}

const std::vector<QVector3D*>& WaterSurfaceElevationPoints::rightBankPoints() const
{
	return m_rightBankPoints;
}

std::vector<QVector3D*>& WaterSurfaceElevationPoints::rightBankPoints()
{
	return m_rightBankPoints;
}

const std::vector<QVector3D*>& WaterSurfaceElevationPoints::arbitraryPoints() const
{
	return m_arbitraryPoints;
}

std::vector<QVector3D*> WaterSurfaceElevationPoints::arbitraryPoints()
{
	return m_arbitraryPoints;
}
