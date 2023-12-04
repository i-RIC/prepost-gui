#include "geodatapolygon.h"
#include "geodatapolygonabstractpolygon.h"
#include "private/geodatapolygonabstractpolygon_impl.h"
#include "private/geodatapolygon_impl.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkMapper.h>
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
	m_active {false},
	m_parent {parent},
	m_linesActor {vtkActor::New()},
	m_pointsActor {vtkActor::New()},
	m_polygonController {},
	m_linesScalarValues {vtkDoubleArray::New()},
	m_pointsScalarValues {vtkDoubleArray::New()}
{
	setupScalarValues();

	auto v = parent->dataModel()->graphicsView();
	m_polygonController.linesActor()->GetProperty()->SetLineWidth(2 * v->devicePixelRatioF());
	m_pointsActor->GetProperty()->SetPointSize(5 * v->devicePixelRatioF());
}

GeoDataPolygonAbstractPolygon::Impl::~Impl()
{
	m_linesActor->Delete();
	m_pointsActor->Delete();
	m_linesScalarValues->Delete();
	m_pointsScalarValues->Delete();
}

void GeoDataPolygonAbstractPolygon::Impl::setupScalarValues()
{
	m_linesScalarValues->SetName(SCALARNAME.c_str());
	auto linesCD = m_polygonController.linesPolyData()->GetCellData();
	linesCD->AddArray(m_linesScalarValues);
	linesCD->SetActiveScalars(SCALARNAME.c_str());

	m_pointsScalarValues->SetName(SCALARNAME.c_str());
	auto pointsCD = m_polygonController.pointsPolyData()->GetCellData();
	pointsCD->AddArray(m_pointsScalarValues);
	pointsCD->SetActiveScalars(SCALARNAME.c_str());
}

GeoDataPolygonAbstractPolygon::GeoDataPolygonAbstractPolygon(GeoDataPolygon* parent) :
	impl {new Impl {parent}}
{
	impl->m_polygonController.pointsActor()->VisibilityOff();
	auto r = parent->renderer();
	r->AddActor(impl->m_polygonController.paintActor());
	r->AddActor(impl->m_linesActor);
	r->AddActor(impl->m_pointsActor);
}

GeoDataPolygonAbstractPolygon::~GeoDataPolygonAbstractPolygon()
{
	auto r = impl->m_parent->renderer();
	r->RemoveActor(impl->m_polygonController.paintActor());
	r->RemoveActor(impl->m_linesActor);
	r->RemoveActor(impl->m_pointsActor);

	auto col = impl->m_parent->actorCollection();
	col->RemoveItem(impl->m_polygonController.paintActor());
	col->RemoveItem(impl->m_linesActor);
	col->RemoveItem(impl->m_pointsActor);

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
	updateActorSetting();
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

vtkPolyData* GeoDataPolygonAbstractPolygon::edgesPolyData() const
{
	return impl->m_polygonController.linesPolyData();
}

void GeoDataPolygonAbstractPolygon::applyVtkPolygonShape()
{
	return impl->m_polygonController.applyVtkPolygonShape();
}

void GeoDataPolygonAbstractPolygon::setZDepthRange(double /*min*/, double max)
{
	impl->m_pointsActor->SetPosition(0, 0, max);
	impl->m_linesActor->SetPosition(0, 0, max);
}

void GeoDataPolygonAbstractPolygon::updateScalarValues()
{
	auto att = impl->m_parent->geoDataGroupDataItem()->condition();
	double val = att->colorMapValue(impl->m_parent->variantValue()).toDouble();
	impl->m_linesScalarValues->Reset();
	for (int i = 0; i < impl->m_polygonController.linesPolyData()->GetNumberOfLines(); ++i) {
		impl->m_linesScalarValues->InsertNextValue(val);
	}
	impl->m_linesScalarValues->Modified();

	impl->m_pointsScalarValues->Reset();
	for (int i = 0; i < impl->m_polygonController.pointsPolyData()->GetNumberOfVerts(); ++i) {
		impl->m_pointsScalarValues->InsertNextValue(val);
	}
	impl->m_pointsScalarValues->Modified();
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
	impl->m_active = active;
	updateActorSetting();
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

void GeoDataPolygonAbstractPolygon::updateActorSetting()
{
	const auto& ds = impl->m_parent->impl->m_displaySetting;
	auto col = impl->m_parent->actorCollection();

	col->RemoveItem(impl->m_linesActor);
	col->RemoveItem(impl->m_pointsActor);

	// color
	impl->m_linesActor->GetProperty()->SetColor(ds.color);
	impl->m_pointsActor->GetProperty()->SetColor(ds.color);

	// mapping
	auto cm = impl->m_parent->colorMapSettingContainer();
	if (ds.mapping == GeoDataPolygon::DisplaySetting::Mapping::Arbitrary || (cm == nullptr)) {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(impl->m_polygonController.linesPolyData());
		impl->m_linesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(impl->m_polygonController.pointsPolyData());
		impl->m_pointsActor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkMapper* mapper = nullptr;

		mapper = cm->buildCellDataMapper(impl->m_polygonController.linesPolyData(), true);
		impl->m_linesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = cm->buildCellDataMapper(impl->m_polygonController.pointsPolyData(), true);
		impl->m_pointsActor->SetMapper(mapper);
		mapper->Delete();
	}

	col->AddItem(impl->m_linesActor);

	// line width and point size
	if (impl->m_active) {
		impl->m_linesActor->GetProperty()->SetLineWidth(ds.lineWidth * 2);
		impl->m_pointsActor->GetProperty()->SetPointSize(ds.lineWidth * 5);
		col->AddItem(impl->m_pointsActor);
	} else {
		impl->m_linesActor->GetProperty()->SetLineWidth(ds.lineWidth);
		impl->m_pointsActor->VisibilityOff();
	}
}

const PolygonController& GeoDataPolygonAbstractPolygon::polygonController() const
{
	return impl->m_polygonController;
}
