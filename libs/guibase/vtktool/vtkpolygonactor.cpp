#include "vtkpolygonactor.h"
#include "private/vtkpolygonactor_impl.h"

#include <vtkPoints.h>
#include <vtkPolygon.h>

#include <QPolygonF>

#include <vector>

vtkPolygonActor::Impl::Impl() :
	m_vtkPolygon {vtkPolygon::New()}
{}

vtkPolygonActor::Impl::~Impl()
{
	m_vtkPolygon->Delete();
}

vtkPolygonActor::vtkPolygonActor() :
	impl {new Impl()}
{}

vtkPolygonActor::~vtkPolygonActor()
{
	delete impl;
}

vtkActor* vtkPolygonActor::pointsActor()
{
	return impl->m_actor.pointsActor();
}

vtkActor* vtkPolygonActor::linesActor()
{
	return impl->m_actor.linesActor();
}

vtkActor* vtkPolygonActor::paintActor()
{
	return impl->m_actor.paintActor();
}

vtkPolyData* vtkPolygonActor::pointsPolyData() const
{
	return impl->m_actor.pointsPolyData();
}

vtkPolyData* vtkPolygonActor::linesPolyData() const
{
	return impl->m_actor.linesPolyData();
}

vtkPolyData* vtkPolygonActor::polygonPolyData() const
{
	return impl->m_actor.polygonsPolyData();
}

QPolygonF vtkPolygonActor::polygon() const
{
	auto points = impl->m_vtkPolygon->GetPoints();
	QPolygonF pol;
	double v[3];

	if (points->GetNumberOfPoints() == 0) {return pol;}

	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, v);
		pol.push_back(QPointF(v[0], v[1]));
	}
	points->GetPoint(0, v);
	pol.push_back(QPointF(v[0], v[1]));

	return pol;
}

void vtkPolygonActor::setPolygon(const QPolygonF& polygon)
{
	auto vtkPol = impl->m_vtkPolygon;
	auto points = vtkPol->GetPoints();
	auto ids = vtkPol->GetPointIds();

	vtkPol->Initialize();
	points->SetNumberOfPoints(0);
	ids->SetNumberOfIds(0);
	for (int i = 0; i < polygon.size() - 1; ++i) {
		const auto& p = polygon.at(i);
		points->InsertNextPoint(p.x(), p.y(), 0);
		ids->InsertNextId(i);
	}
	applyVtkPolygonShape();
}

vtkPolygon* vtkPolygonActor::getVtkPolygon() const
{
	return impl->m_vtkPolygon;
}

void vtkPolygonActor::applyVtkPolygonShape()
{
	std::vector<QPolygonF> polygons;
	polygons.push_back(polygon());
	impl->m_actor.setPolygons(polygons);
}
