#include "geodatapolygon.h"
#include "geodatapolygonabstractpolygon.h"

#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <QPolygonF>

#include <string>

namespace {

const std::string SCALARNAME = "polygonvalue";

}

GeoDataPolygonAbstractPolygon::GeoDataPolygonAbstractPolygon(GeoDataPolygon* parent) :
	m_selectedVertexId {0},
	m_selectedEdgeId {0},
	m_parent {parent},
	m_scalarValues {vtkDoubleArray::New()}
{
	setupScalarValues();

	m_polygonController.linesActor()->GetProperty()->SetLineWidth(2);
	m_polygonController.pointsActor()->GetProperty()->SetPointSize(5);

	m_polygonController.pointsActor()->VisibilityOff();
	auto r = parent->renderer();
	r->AddActor(m_polygonController.paintActor());
	r->AddActor(m_polygonController.linesActor());
	r->AddActor(m_polygonController.pointsActor());

	auto col = parent->actorCollection();
	col->AddItem(m_polygonController.paintActor());
	col->AddItem(m_polygonController.linesActor());

	parent->updateVisibilityWithoutRendering();
}

GeoDataPolygonAbstractPolygon::~GeoDataPolygonAbstractPolygon()
{
	auto r = m_parent->renderer();
	r->RemoveActor(m_polygonController.paintActor());
	r->RemoveActor(m_polygonController.linesActor());
	r->RemoveActor(m_polygonController.pointsActor());

	auto col = m_parent->actorCollection();
	col->RemoveItem(m_polygonController.paintActor());
	col->RemoveItem(m_polygonController.linesActor());
	col->RemoveItem(m_polygonController.pointsActor());

	m_scalarValues->Delete();
}

QPolygonF GeoDataPolygonAbstractPolygon::polygon(const QPointF& offset) const
{
	QPolygonF pol = m_polygonController.polygon();
	for (QPointF& p : pol) {
		p.setX(p.x() + offset.x());
		p.setY(p.y() + offset.y());
	}
	return pol;
}

void GeoDataPolygonAbstractPolygon::setPolygon(const QPolygonF& p)
{
	m_polygonController.setPolygon(p);
	updateScalarValues();
}

QPolygonF GeoDataPolygonAbstractPolygon::cleanedPolygon(const QPointF& offset) const
{
	QPolygonF pol = polygon(offset);
	int idx = 1;
	while (idx < pol.size()) {
		if (pol.at(idx) == pol.at(idx - 1)) {
			pol.remove(idx);
		}
	}
	return pol;
}

bool GeoDataPolygonAbstractPolygon::isVertexSelectable(const QPointF& pos, double distlimit)
{
	return m_polygonController.isVertexSelectable(pos, distlimit, &m_selectedVertexId);
}

bool GeoDataPolygonAbstractPolygon::isEdgeSelectable(const QPointF& pos, double distlimit)
{
	return m_polygonController.isEdgeSelectable(pos, distlimit, &m_selectedEdgeId);
}

bool GeoDataPolygonAbstractPolygon::isPolygonSelectable(const QPointF& pos)
{
	return m_polygonController.isAreaSelectable(pos);
}

vtkPolygon* GeoDataPolygonAbstractPolygon::getVtkPolygon() const
{
	return m_polygonController.getVtkPolygon();
}

void GeoDataPolygonAbstractPolygon::applyVtkPolygonShape()
{
	return m_polygonController.applyVtkPolygonShape();
}

void GeoDataPolygonAbstractPolygon::setZDepthRange(double /*min*/, double max)
{
	m_polygonController.pointsActor()->SetPosition(0, 0, max);
	m_polygonController.linesActor()->SetPosition(0, 0, max);
}

void GeoDataPolygonAbstractPolygon::updateScalarValues()
{
	double val = m_parent->variantValue().toDouble();
	m_scalarValues->Reset();
	for (int i = 0; i < m_polygonController.getVtkPolygon()->GetNumberOfPoints(); ++i) {
		m_scalarValues->InsertNextValue(val);
	}
	m_scalarValues->Modified();
}

int GeoDataPolygonAbstractPolygon::selectedVertexId() const
{
	return m_selectedVertexId;
}

int GeoDataPolygonAbstractPolygon::selectedEdgeId() const
{
	return m_selectedEdgeId;
}

void GeoDataPolygonAbstractPolygon::setActive(bool active)
{
	auto col = m_parent->actorCollection();
	auto pointsActor = m_polygonController.pointsActor();
	auto linesActor = m_polygonController.linesActor();
	col->RemoveItem(pointsActor);
	if (active) {
		col->AddItem(pointsActor);
		linesActor->GetProperty()->SetLineWidth(GeoDataPolygon::selectedEdgeWidth);
	} else {
		pointsActor->VisibilityOff();
		linesActor->GetProperty()->SetLineWidth(GeoDataPolygon::normalEdgeWidth);
	}
	m_parent->updateVisibilityWithoutRendering();
}

void GeoDataPolygonAbstractPolygon::setSelected(bool selected)
{
	auto col = m_parent->actorCollection();
	auto paintActor = m_polygonController.paintActor();
	col->RemoveItem(paintActor);
	if (selected) {
		col->AddItem(paintActor);
	} else {
		paintActor->VisibilityOff();
	}
	m_parent->updateVisibilityWithoutRendering();
}

void GeoDataPolygonAbstractPolygon::finishDefinition()
{}

void GeoDataPolygonAbstractPolygon::setLookupTable(vtkScalarsToColors* t)
{
	m_polygonController.pointsActor()->GetMapper()->SetLookupTable(t);
	m_polygonController.linesActor()->GetMapper()->SetLookupTable(t);
}

void GeoDataPolygonAbstractPolygon::setColor(const QColor& color)
{
	double rate = 0.8;
	double dr = color.redF() * rate;
	double dg = color.greenF() * rate;
	double db = color.blueF() * rate;

	m_polygonController.pointsActor()->GetProperty()->SetColor(dr, dg, db);
	m_polygonController.linesActor()->GetProperty()->SetColor(dr, dg, db);
}

void GeoDataPolygonAbstractPolygon::setMapping(GeoDataPolygonColorSettingDialog::Mapping m)
{
	auto pointsMapper = m_polygonController.pointsActor()->GetMapper();
	auto linesMapper = m_polygonController.linesActor()->GetMapper();

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
	return m_polygonController;
}

void GeoDataPolygonAbstractPolygon::setupScalarValues()
{
	m_scalarValues->SetName(SCALARNAME.c_str());

	auto linesPD = m_polygonController.linesPolyData()->GetPointData();
	linesPD->AddArray(m_scalarValues);
	linesPD->SetActiveScalars(SCALARNAME.c_str());

	auto pointsPD = m_polygonController.pointsPolyData()->GetPointData();
	pointsPD->AddArray(m_scalarValues);
	pointsPD->SetActiveScalars(SCALARNAME.c_str());
}
