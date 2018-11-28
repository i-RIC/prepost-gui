#include "geodatapolyline.h"
#include "geodatapolylineabstractpolyline.h"
#include "private/geodatapolylineabstractpolyline_impl.h"

#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkMapper.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <string>

namespace {

const std::string SCALARNAME = "polylinevalue";

}

GeoDataPolyLineAbstractPolyLine::Impl::Impl(GeoDataPolyLine* parent) :
	m_selectedVertexId {0},
	m_selectedEdgeId {0},
	m_parent {parent},
	m_polylineController {},
	m_scalarValues {vtkDoubleArray::New()}
{
	setupScalarValues();

	m_polylineController.linesActor()->GetProperty()->SetLineWidth(2);
	m_polylineController.pointsActor()->GetProperty()->SetPointSize(5);
}

void GeoDataPolyLineAbstractPolyLine::Impl::setupScalarValues()
{
	m_scalarValues->SetName(SCALARNAME.c_str());

	auto linesPD = m_polylineController.polyData()->GetPointData();
	linesPD->AddArray(m_scalarValues);
	linesPD->SetActiveScalars(SCALARNAME.c_str());

	auto pointsPD = m_polylineController.pointsPolyData()->GetPointData();
	pointsPD->AddArray(m_scalarValues);
	pointsPD->SetActiveScalars(SCALARNAME.c_str());
}

GeoDataPolyLineAbstractPolyLine::GeoDataPolyLineAbstractPolyLine(GeoDataPolyLine* parent) :
	impl {new Impl {parent}}
{
	impl->m_polylineController.pointsActor()->VisibilityOff();
	auto r = parent->renderer();
	r->AddActor(impl->m_polylineController.linesActor());
	r->AddActor(impl->m_polylineController.pointsActor());

	auto col = parent->actorCollection();
	col->AddItem(impl->m_polylineController.linesActor());

	parent->updateVisibilityWithoutRendering();
}

GeoDataPolyLineAbstractPolyLine::~GeoDataPolyLineAbstractPolyLine()
{
	auto r = impl->m_parent->renderer();
	r->RemoveActor(impl->m_polylineController.linesActor());
	r->RemoveActor(impl->m_polylineController.pointsActor());

	auto col = impl->m_parent->actorCollection();
	col->RemoveItem(impl->m_polylineController.linesActor());
	col->RemoveItem(impl->m_polylineController.pointsActor());

	impl->m_scalarValues->Delete();
	delete impl;
}

std::vector<QPointF> GeoDataPolyLineAbstractPolyLine::polyLine(const QPointF& offset) const
{
	auto pol = impl->m_polylineController.polyLine();
	for (QPointF& p : pol) {
		p.setX(p.x() + offset.x());
		p.setY(p.y() + offset.y());
	}
	return pol;
}

void GeoDataPolyLineAbstractPolyLine::setPolyLine(const std::vector<QPointF>& p)
{
	impl->m_polylineController.setPolyLine(p);
	updateScalarValues();
}

std::vector<QPointF> GeoDataPolyLineAbstractPolyLine::cleanedPolyLine(const QPointF& offset) const
{
	auto pol = polyLine(offset);
	int idx = 1;
	while (idx < pol.size()) {
		if (pol.at(idx) == pol.at(idx - 1)) {
			pol.erase(pol.begin() + idx);
		}
	}
	return pol;
}

bool GeoDataPolyLineAbstractPolyLine::isVertexSelectable(const QPointF& pos, double distlimit)
{
	return impl->m_polylineController.isVertexSelectable(pos, distlimit, &impl->m_selectedVertexId);
}

bool GeoDataPolyLineAbstractPolyLine::isEdgeSelectable(const QPointF& pos, double distlimit)
{
	return impl->m_polylineController.isEdgeSelectable(pos, distlimit, &impl->m_selectedEdgeId);
}

void GeoDataPolyLineAbstractPolyLine::setZDepthRange(double /*min*/, double max)
{
	impl->m_polylineController.pointsActor()->SetPosition(0, 0, max);
	impl->m_polylineController.linesActor()->SetPosition(0, 0, max);
}

void GeoDataPolyLineAbstractPolyLine::updateScalarValues()
{
	double val = impl->m_parent->variantValue().toDouble();
	impl->m_scalarValues->Reset();
	for (int i = 0; i < impl->m_polylineController.polyLine().size(); ++i) {
		impl->m_scalarValues->InsertNextValue(val);
	}
	impl->m_scalarValues->Modified();
}

int GeoDataPolyLineAbstractPolyLine::selectedVertexId() const
{
	return impl->m_selectedVertexId;
}

int GeoDataPolyLineAbstractPolyLine::selectedEdgeId() const
{
	return impl->m_selectedEdgeId;
}

void GeoDataPolyLineAbstractPolyLine::setActive(bool active)
{
	auto col = impl->m_parent->actorCollection();
	auto pointsActor = impl->m_polylineController.pointsActor();
	auto linesActor = impl->m_polylineController.linesActor();
	col->RemoveItem(pointsActor);
	if (active) {
		col->AddItem(pointsActor);
		linesActor->GetProperty()->SetLineWidth(GeoDataPolyLine::selectedEdgeWidth);
	} else {
		pointsActor->VisibilityOff();
		linesActor->GetProperty()->SetLineWidth(GeoDataPolyLine::normalEdgeWidth);
	}
	impl->m_parent->updateVisibilityWithoutRendering();
}

void GeoDataPolyLineAbstractPolyLine::finishDefinition()
{}

void GeoDataPolyLineAbstractPolyLine::setLookupTable(vtkScalarsToColors* t)
{
	impl->m_polylineController.pointsActor()->GetMapper()->SetLookupTable(t);
	impl->m_polylineController.linesActor()->GetMapper()->SetLookupTable(t);
}

void GeoDataPolyLineAbstractPolyLine::setColor(const QColor& color)
{
	double rate = 0.8;
	double dr = color.redF() * rate;
	double dg = color.greenF() * rate;
	double db = color.blueF() * rate;

	impl->m_polylineController.pointsActor()->GetProperty()->SetColor(dr, dg, db);
	impl->m_polylineController.linesActor()->GetProperty()->SetColor(dr, dg, db);
}

void GeoDataPolyLineAbstractPolyLine::setMapping(GeoDataPolyLineColorSettingDialog::Mapping m)
{
	auto pointsMapper = impl->m_polylineController.pointsActor()->GetMapper();
	auto linesMapper = impl->m_polylineController.linesActor()->GetMapper();

	if (m == GeoDataPolyLineColorSettingDialog::Arbitrary) {
		pointsMapper->SetScalarVisibility(0);
		linesMapper->SetScalarVisibility(0);
	} else {
		pointsMapper->SetScalarVisibility(1);
		linesMapper->SetScalarVisibility(1);
	}
}

const PolyLineController& GeoDataPolyLineAbstractPolyLine::polylineController() const
{
	return impl->m_polylineController;
}
