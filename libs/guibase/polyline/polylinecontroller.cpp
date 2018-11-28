#include "polylinecontroller.h"

#include "private/polylinecontroller_impl.h"

#include <vtkPolyData.h>

#include <QPointF>
#include <QVector2D>

PolyLineController::Impl::Impl() :
	m_actor {}
{}

PolyLineController::PolyLineController() :
	impl {new Impl {}}
{}

PolyLineController::~PolyLineController()
{
	delete impl;
}

std::vector<QPointF> PolyLineController::polyLine() const
{
	return impl->m_actor.line();
}

void PolyLineController::setPolyLine(const std::vector<QPointF>& polyLine)
{
	impl->m_actor.setLine(polyLine);
}

bool PolyLineController::isVertexSelectable(const QPointF& pos, double limitDistance, int* vid)
{
	auto polydata = impl->m_actor.pointsPolyData();
	*vid = polydata->FindPoint(pos.x(), pos.y(), 0.0);

	if (*vid == -1) {return false;}

	double v[3];
	polydata->GetPoint(static_cast<vtkIdType>(*vid), v);
	QVector2D diff(pos.x() - v[0], pos.y() - v[1]);

	return diff.lengthSquared() < limitDistance * limitDistance;
}

bool PolyLineController::isEdgeSelectable(const QPointF& pos, double limitDistance, int* edgeId)
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

vtkPolyData* PolyLineController::pointsPolyData() const
{
	return impl->m_actor.pointsPolyData();
}

vtkPolyData* PolyLineController::polyData() const
{
	return impl->m_actor.linesPolyData();
}

vtkActor* PolyLineController::pointsActor() const
{
	return impl->m_actor.pointsActor();
}

vtkActor* PolyLineController::linesActor() const
{
	return impl->m_actor.lineActor();
}
