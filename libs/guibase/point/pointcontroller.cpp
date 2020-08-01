#include "pointcontroller.h"
#include "private/pointcontroller_impl.h"

#include <QVector2D>

PointController::Impl::Impl() :
	m_actor {}
{}

PointController::PointController() :
	impl {new Impl {}}
{}

PointController::~PointController()
{
	delete impl;
}

QPointF PointController::point() const
{
	auto l = impl->m_actor.line();
	if (l.size() == 0) {return QPointF();}

	return l.at(0);
}

void PointController::setPoint(const QPointF& point)
{
	std::vector<QPointF> line;
	line.push_back(point);
	impl->m_actor.setLine(line);
}

void PointController::clear()
{
	std::vector<QPointF> empty;
	impl->m_actor.setLine(empty);
}

bool PointController::pointIsSet() const
{
	return impl->m_actor.line().size() > 0;
}

bool PointController::isSelectable(const QPointF& pos, double limitDistance)
{
	auto l = impl->m_actor.line();
	if (l.size() == 0) {return false;}

	auto p = l.at(0);
	QVector2D diff(pos.x() - p.x(), pos.y() - p.y());
	return diff.lengthSquared() < limitDistance * limitDistance;
}

vtkPolyData* PointController::polyData()
{
	return impl->m_actor.pointsPolyData();
}

vtkActor* PointController::actor() const
{
	return impl->m_actor.pointsActor();
}
