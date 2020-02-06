#include "polylinecontroller.h"

#include "private/polylinecontroller_impl.h"

#include <misc/mathsupport.h>

#include <vtkPolyData.h>

#include <QPointF>
#include <QVector2D>

PolyLineController::Impl::Impl() :
	m_actor {}
{}


QPointF PolyLineController::Impl::nearestPoint(int lineId, const QPointF& point)
{
	auto l = m_actor.line();
	auto a = l.at(lineId);
	auto b = l.at(lineId + 1);
	return nearestPoint(a, b, point);
}

QPointF PolyLineController::Impl::nearestPoint(const QPointF& a, const QPointF& b, const QPointF& p)
{
	double abx = b.x() - a.x();
	double aby = b.y() - a.y();
	double apx = p.x() - a.x();
	double apy = p.y() - a.y();

	double ab_len = sqrt(abx * abx + aby * aby);
	if (ab_len == 0) {return a;}

	double abunitx = abx / ab_len;
	double abunity = aby / ab_len;

	double r = (apx * abunitx + apy* abunity) / ab_len;
	if (r < 0) {
		return a;
	} else if (r > 1) {
		return b;
	} else {
		return a + (b - a) * r;
	}
}

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

void PolyLineController::clear()
{
	std::vector<QPointF> empty;
	setPolyLine(empty);
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

int PolyLineController::findNearestLine(const QPointF& pos)
{
	int edgeId = 0;
	double minDistSquared;
	QPointF ret;
	int lineSize = polyLine().size() - 1;
	for (int i = 0; i < lineSize; ++i) {
		ret = impl->nearestPoint(i, pos);
		double distSquared = iRIC::lengthSquared(pos - ret);
		if (i == 0 || distSquared < minDistSquared) {
			minDistSquared = distSquared;
			edgeId = i;
		}
	}
	return edgeId;
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
