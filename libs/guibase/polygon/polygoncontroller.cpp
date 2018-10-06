#include "polygoncontroller.h"
#include "private/polygoncontroller_impl.h"

#include <vtkPolyData.h>

#include <QPointF>
#include <QPolygonF>
#include <QVector2D>

PolygonController::Impl::Impl() :
	m_actor {}
{}

PolygonController::PolygonController() :
	impl {new Impl {}}
{}

PolygonController::~PolygonController()
{
	delete impl;
}

QPolygonF PolygonController::polygon() const
{
	return impl->m_actor.polygon();
}

void PolygonController::setPolygon(const QPolygonF& polygon)
{
	return impl->m_actor.setPolygon(polygon);
}

vtkPolygon* PolygonController::getVtkPolygon() const
{
	return impl->m_actor.getVtkPolygon();
}

void PolygonController::applyVtkPolygonShape()
{
	impl->m_actor.applyVtkPolygonShape();
}

bool PolygonController::isVertexSelectable(const QPointF& pos, double limitDistance, int* vid)
{
	auto polydata = impl->m_actor.pointsPolyData();
	*vid = polydata->FindPoint(pos.x(), pos.y(), 0.0);

	if (*vid == -1) {return false;}

	double v[3];
	polydata->GetPoint(static_cast<vtkIdType>(*vid), v);
	QVector2D diff(pos.x() - v[0], pos.y() - v[1]);

	return diff.lengthSquared() < limitDistance * limitDistance;
}

bool PolygonController::isEdgeSelectable(const QPointF& pos, double limitDistance, int* edgeId)
{
	double x[3] = {pos.x(), pos.y(), 0};
	int subId;
	double pcoords[3];
	double weights[32];

	double d2 = limitDistance * limitDistance;

	vtkIdType id = impl->m_actor.linesPolyData()->FindCell(x, nullptr, 0, d2, subId, pcoords, weights);
	if (id < 0) {return false;}

	*edgeId = id;
	return true;
}

bool PolygonController::isAreaSelectable(const QPointF& pos)
{
	auto pol = impl->m_actor.polygon();
	return pol.containsPoint(pos, Qt::OddEvenFill);
}

vtkPolyData* PolygonController::pointsPolyData() const
{
	return impl->m_actor.pointsPolyData();
}

vtkPolyData* PolygonController::linesPolyData() const
{
	return impl->m_actor.linesPolyData();
}

vtkPolyData* PolygonController::polyData() const
{
	return impl->m_actor.polygonPolyData();
}

vtkActor* PolygonController::pointsActor() const
{
	return impl->m_actor.pointsActor();
}

vtkActor* PolygonController::linesActor() const
{
	return impl->m_actor.linesActor();
}

vtkActor* PolygonController::paintActor() const
{
	return impl->m_actor.paintActor();
}
