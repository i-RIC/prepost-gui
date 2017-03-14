#include "watersurfaceelevationpoints.h"
#include "../project/project.h"
#include "../../io/points/pointsimporter.h"

#include <QStandardItem>
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
	clearPoints();
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

void WaterSurfaceElevationPoints::clearPoints()
{
	clearVector(&m_leftBankPoints);
	clearVector(&m_rightBankPoints);
	clearVector(&m_arbitraryPoints);
}

void WaterSurfaceElevationPoints::importData(QWidget* w)
{
	std::vector<QVector3D*> newPoints;
	bool ok = PointsImporter::importData(&newPoints, &(project()->offset()), w);
	if (! ok) {return;}

	clearPoints();
	m_arbitraryPoints = newPoints;
}

void WaterSurfaceElevationPoints::exportData(QWidget* w)
{

}

QStandardItem* WaterSurfaceElevationPoints::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(tr("Water Elevation Points"));
	setupStandardItem(item);
	return item;
}
