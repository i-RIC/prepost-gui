#include "points.h"
#include "pointspreprocessorcontroller.h"
#include "../project/project.h"

#include "private/points_impl.h"

#include <QVector3D>

Points::Points(DataItem* parent) :
	DataItem {parent},
	impl {new Impl {}}
{}

Points::~Points()
{
	clearPoints();
	delete impl;
}

const std::vector<QVector3D*>& Points::points() const
{
	return impl->m_points;
}

std::vector<QVector3D*>& Points::points()
{
	return impl->m_points;
}

void Points::setPoints(const std::vector<QVector3D*>& points)
{
	clearPoints();
	impl->m_points = points;
}

void Points::clearPoints()
{
	for (auto p : impl->m_points) {
		delete p;
	}
	impl->m_points.clear();
}

DataItemController* Points::buildPreProcessorDataItemController(Model* model)
{
	return new PointsPreProcessorController(model, this);
}
