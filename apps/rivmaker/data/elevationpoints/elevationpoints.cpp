#include "elevationpoints.h"
#include "elevationpointspreprocessorview.h"
#include "../project/project.h"
#include "../../io/points/pointsimporter.h"

#include <QStandardItem>
#include <QVector3D>

ElevationPoints::ElevationPoints(DataItem *parent) :
	DataItem {parent}
{}

ElevationPoints::~ElevationPoints()
{
	clearPoints();
}

const std::vector<QVector3D*>& ElevationPoints::points() const
{
	return m_points;
}

std::vector<QVector3D*>& ElevationPoints::points()
{
	return m_points;
}

void ElevationPoints::clearPoints()
{
	for (auto p : m_points) {
		delete p;
	}
	m_points.clear();
}

void ElevationPoints::importData(QWidget* w)
{
	std::vector<QVector3D*> newPoints;
	bool ok = PointsImporter::importData(&newPoints, &(project()->offset()), w);
	if (! ok) {return;}

	clearPoints();
	m_points = newPoints;
}

void ElevationPoints::exportData(QWidget* w)
{

}

QStandardItem* ElevationPoints::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(tr("Elevation Points"));
	setupStandardItem(item);
	return item;
}

DataItemView* ElevationPoints::buildPreProcessorDataItemView(Model* model)
{
	return new ElevationPointsPreprocessorView(model, this);
}
