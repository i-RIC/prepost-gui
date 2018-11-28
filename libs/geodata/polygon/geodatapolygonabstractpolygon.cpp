#include "geodatapolygon.h"
#include "geodatapolygonabstractpolygon.h"
#include "private/geodatapolygonabstractpolygon_impl.h"

#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkMapper.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <QPolygonF>

#include <string>

namespace {

const std::string SCALARNAME = "polygonvalue";

}

GeoDataPolygonAbstractPolygon::Impl::Impl(GeoDataPolygon* parent) :
	m_selectedVertexId {0},
	m_selectedEdgeId {0},
	m_parent {parent},
	m_polygonController {},
	m_scalarValues {vtkDoubleArray::New()}
{
	setupScalarValues();

	m_polygonController.linesActor()->GetProperty()->SetLineWidth(2);
	m_polygonController.pointsActor()->GetProperty()->SetPointSize(5);
}

void GeoDataPolygonAbstractPolygon::Impl::setupScalarValues()
{
	m_scalarValues->SetName(SCALARNAME.c_str());

	auto linesPD = m_polygonController.linesPolyData()->GetPointData();
	linesPD->AddArray(m_scalarValues);
	linesPD->SetActiveScalars(SCALARNAME.c_str());

	auto pointsPD = m_polygonController.pointsPolyData()->GetPointData();
	pointsPD->AddArray(m_scalarValues);
	pointsPD->SetActiveScalars(SCALARNAME.c_str());
}

GeoDataPolygonAbstractPolygon::GeoDataPolygonAbstractPolygon(GeoDataPolygon* parent) :
	impl {new Impl {parent}}
{
	impl->m_polygonController.pointsActor()->VisibilityOff();
	auto r = parent->renderer();
	r->AddActor(impl->m_polygonController.paintActor());
	r->AddActor(impl->m_polygonController.linesActor());
	r->AddActor(impl->m_polygonController.pointsActor());

	auto col = parent->actorCollection();
	col->AddItem(impl->m_polygonController.paintActor());
	col->AddItem(impl->m_polygonController.linesActor());

	parent->updateVisibilityWithoutRendering();
}

GeoDataPolygonAbstractPolygon::~GeoDataPolygonAbstractPolygon()
{
	auto r = impl->m_parent->renderer();
	r->RemoveActor(impl->m_polygonController.paintActor());
	r->RemoveActor(impl->m_polygonController.linesActor());
	r->RemoveActor(impl->m_polygonController.pointsActor());

	auto col = impl->m_parent->actorCollection();
	col->RemoveItem(impl->m_polygonController.paintActor());
	col->RemoveItem(impl->m_polygonController.linesActor());
	col->RemoveItem(impl->m_polygonController.pointsActor());

	impl->m_scalarValues->Delete();
	delete impl;
}

QPolygonF GeoDataPolygonAbstractPolygon::polygon(const QPointF& offset) const
{
	QPolygonF pol = impl->m_polygonController.polygon();
	for (QPointF& p : pol) {
		p.setX(p.x() + offset.x());
		p.setY(p.y() + offset.y());
	}
	return pol;
}

void GeoDataPolygonAbstractPolygon::setPolygon(const QPolygonF& p)
{
	impl->m_polygonController.setPolygon(p);
	updateScalarValues();
}

QPolygonF GeoDataPolygonAbstractPolygon::cleanedPolygon(const QPointF& offset) const
{
	QPolygonF pol = polygon(offset);
	int idx = 1;
	while (idx < pol.size()) {
		if (pol.at(idx) == pol.at(idx - 1)) {
			pol.remove(idx);
			continue;
		}
		++ idx;
	}
	return pol;
}

bool GeoDataPolygonAbstractPolygon::isVertexSelectable(const QPointF& pos, double distlimit)
{
	return impl->m_polygonController.isVertexSelectable(pos, distlimit, &impl->m_selectedVertexId);
}

bool GeoDataPolygonAbstractPolygon::isEdgeSelectable(const QPointF& pos, double distlimit)
{
	return impl->m_polygonController.isEdgeSelectable(pos, distlimit, &impl->m_selectedEdgeId);
}

bool GeoDataPolygonAbstractPolygon::isPolygonSelectable(const QPointF& pos)
{
	return impl->m_polygonController.isAreaSelectable(pos);
}

vtkPolygon* GeoDataPolygonAbstractPolygon::getVtkPolygon() const
{
	return impl->m_polygonController.getVtkPolygon();
}

void GeoDataPolygonAbstractPolygon::applyVtkPolygonShape()
{
	return impl->m_polygonController.applyVtkPolygonShape();
}

void GeoDataPolygonAbstractPolygon::setZDepthRange(double /*min*/, double max)
{
	impl->m_polygonController.pointsActor()->SetPosition(0, 0, max);
	impl->m_polygonController.linesActor()->SetPosition(0, 0, max);
}

void GeoDataPolygonAbstractPolygon::updateScalarValues()
{
	double val = impl->m_parent->variantValue().toDouble();
	impl->m_scalarValues->Reset();
	for (int i = 0; i < impl->m_polygonController.getVtkPolygon()->GetNumberOfPoints(); ++i) {
		impl->m_scalarValues->InsertNextValue(val);
	}
	impl->m_scalarValues->Modified();
}

int GeoDataPolygonAbstractPolygon::selectedVertexId() const
{
	return impl->m_selectedVertexId;
}

int GeoDataPolygonAbstractPolygon::selectedEdgeId() const
{
	return impl->m_selectedEdgeId;
}

void GeoDataPolygonAbstractPolygon::setActive(bool active)
{
	auto col = impl->m_parent->actorCollection();
	auto pointsActor = impl->m_polygonController.pointsActor();
	auto linesActor = impl->m_polygonController.linesActor();
	col->RemoveItem(pointsActor);
	if (active) {
		col->AddItem(pointsActor);
		linesActor->GetProperty()->SetLineWidth(GeoDataPolygon::selectedEdgeWidth);
	} else {
		pointsActor->VisibilityOff();
		linesActor->GetProperty()->SetLineWidth(GeoDataPolygon::normalEdgeWidth);
	}
	impl->m_parent->updateVisibilityWithoutRendering();
}

void GeoDataPolygonAbstractPolygon::setSelected(bool selected)
{
	auto col = impl->m_parent->actorCollection();
	auto paintActor = impl->m_polygonController.paintActor();
	col->RemoveItem(paintActor);
	if (selected) {
		col->AddItem(paintActor);
	} else {
		paintActor->VisibilityOff();
	}
	impl->m_parent->updateVisibilityWithoutRendering();
}

void GeoDataPolygonAbstractPolygon::finishDefinition()
{}

void GeoDataPolygonAbstractPolygon::setLookupTable(vtkScalarsToColors* t)
{
	impl->m_polygonController.pointsActor()->GetMapper()->SetLookupTable(t);
	impl->m_polygonController.linesActor()->GetMapper()->SetLookupTable(t);
}

void GeoDataPolygonAbstractPolygon::setColor(const QColor& color)
{
	double rate = 0.8;
	double dr = color.redF() * rate;
	double dg = color.greenF() * rate;
	double db = color.blueF() * rate;

	impl->m_polygonController.pointsActor()->GetProperty()->SetColor(dr, dg, db);
	impl->m_polygonController.linesActor()->GetProperty()->SetColor(dr, dg, db);
}

void GeoDataPolygonAbstractPolygon::setMapping(GeoDataPolygonColorSettingDialog::Mapping m)
{
	auto pointsMapper = impl->m_polygonController.pointsActor()->GetMapper();
	auto linesMapper = impl->m_polygonController.linesActor()->GetMapper();

	if (m == GeoDataPolygonColorSettingDialog::Arbitrary) {
		pointsMapper->SetScalarVisibility(0);
		linesMapper->SetScalarVisibility(0);
	} else {
		pointsMapper->SetScalarVisibility(1);
		linesMapper->SetScalarVisibility(1);
	}
}

const PolygonController& GeoDataPolygonAbstractPolygon::polygonController() const
{
	return impl->m_polygonController;
}
