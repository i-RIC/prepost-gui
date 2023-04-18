#include "geodatapolyline.h"
#include "geodatapolylineabstractpolyline.h"
#include "private/geodatapolylineabstractpolyline_impl.h"
#include "private/geodatapolyline_impl.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>

#include <vtkActor.h>
#include <vtkDoubleArray.h>
#include <vtkMapper.h>
#include <vtkCellData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
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
	m_linesActor {vtkActor::New()},
	m_pointsActor {vtkActor::New()},
	m_polylineController {},
	m_linesScalarValues {vtkDoubleArray::New()},
	m_pointsScalarValues {vtkDoubleArray::New()}
{
	setupScalarValues();

	m_linesActor->GetProperty()->SetLineWidth(2);
	m_pointsActor->GetProperty()->SetPointSize(5);
}

GeoDataPolyLineAbstractPolyLine::Impl::~Impl()
{
	m_linesActor->Delete();
	m_pointsActor->Delete();
	m_linesScalarValues->Delete();
	m_pointsScalarValues->Delete();
}

void GeoDataPolyLineAbstractPolyLine::Impl::setupScalarValues()
{
	m_linesScalarValues->SetName(SCALARNAME.c_str());
	auto linesCD = m_polylineController.polyData()->GetCellData();
	linesCD->AddArray(m_linesScalarValues);
	linesCD->SetActiveScalars(SCALARNAME.c_str());

	m_pointsScalarValues->SetName(SCALARNAME.c_str());
	auto pointsCD = m_polylineController.pointsPolyData()->GetCellData();
	pointsCD->AddArray(m_pointsScalarValues);
	pointsCD->SetActiveScalars(SCALARNAME.c_str());
}

GeoDataPolyLineAbstractPolyLine::GeoDataPolyLineAbstractPolyLine(GeoDataPolyLine* parent) :
	impl {new Impl {parent}}
{
	impl->m_pointsActor->VisibilityOff();
	auto r = parent->renderer();
	r->AddActor(impl->m_linesActor);
	r->AddActor(impl->m_pointsActor);

	auto col = parent->actorCollection();
	col->AddItem(impl->m_linesActor);

	parent->updateVisibilityWithoutRendering();
}

GeoDataPolyLineAbstractPolyLine::~GeoDataPolyLineAbstractPolyLine()
{
	auto r = impl->m_parent->renderer();
	r->RemoveActor(impl->m_linesActor);
	r->RemoveActor(impl->m_pointsActor);

	auto col = impl->m_parent->actorCollection();
	col->RemoveItem(impl->m_linesActor);
	col->RemoveItem(impl->m_pointsActor);

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
	updateActorSetting();
	emit impl->m_parent->modified();
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

vtkPolyData* GeoDataPolyLineAbstractPolyLine::linePolyData() const
{
	return impl->m_polylineController.polyData();
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
	impl->m_pointsActor->SetPosition(0, 0, max);
	impl->m_linesActor->SetPosition(0, 0, max);
}

void GeoDataPolyLineAbstractPolyLine::updateScalarValues()
{
	double val = impl->m_parent->variantValue().toDouble();
	impl->m_linesScalarValues->Reset();
	for (int i = 0; i < static_cast<int> (impl->m_polylineController.polyLine().size()) - 1; ++i) {
		impl->m_linesScalarValues->InsertNextValue(val);
	}
	impl->m_linesScalarValues->Modified();

	impl->m_pointsScalarValues->Reset();
	for (int i = 0; i < impl->m_polylineController.polyLine().size(); ++i) {
		impl->m_pointsScalarValues->InsertNextValue(val);
	}
	impl->m_pointsScalarValues->Modified();
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
	col->RemoveItem(impl->m_pointsActor);
	if (active) {
		col->AddItem(impl->m_pointsActor);
		updateActorSetting();
	} else {
		impl->m_pointsActor->VisibilityOff();
		impl->m_linesActor->GetProperty()->SetLineWidth(impl->m_parent->impl->m_displaySetting.lineWidth);
	}

	impl->m_parent->updateVisibilityWithoutRendering();
}

void GeoDataPolyLineAbstractPolyLine::finishDefinition()
{}

void GeoDataPolyLineAbstractPolyLine::updateActorSetting()
{
	auto ds = impl->m_parent->impl->m_displaySetting;

	// color
	impl->m_linesActor->GetProperty()->SetColor(ds.color);
	impl->m_pointsActor->GetProperty()->SetColor(ds.color);

	// opacity
	impl->m_linesActor->GetProperty()->SetOpacity(ds.opacity);
	impl->m_pointsActor->GetProperty()->SetOpacity(ds.opacity);

	// lineWidth
	impl->m_linesActor->GetProperty()->SetLineWidth(ds.lineWidth * 2);
	impl->m_pointsActor->GetProperty()->SetPointSize(ds.lineWidth * 5);

	// mapping
	auto cm = impl->m_parent->geoDataDataItem()->colorMapSettingContainer();
	if (ds.mapping == GeoDataPolyLine::DisplaySetting::Mapping::Arbitrary || (cm == nullptr)) {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(impl->m_polylineController.polyData());
		impl->m_linesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(impl->m_polylineController.pointsPolyData());
		impl->m_pointsActor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkMapper* mapper = nullptr;

		mapper = cm->buildCellDataMapper(impl->m_polylineController.polyData(), true);
		impl->m_linesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = cm->buildCellDataMapper(impl->m_polylineController.pointsPolyData(), true);
		impl->m_pointsActor->SetMapper(mapper);
		mapper->Delete();
	}
}

const PolyLineController& GeoDataPolyLineAbstractPolyLine::polylineController() const
{
	return impl->m_polylineController;
}
