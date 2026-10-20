#include "geodatapolygongroup.h"
#include "geodatapolygongroupproxy.h"
#include "private/geodatapolygongroup_impl.h"
#include "private/geodatapolygongroupproxy_displaysettingwidget.h"
#include "private/geodatapolygongroupproxy_impl.h"
#include "public/geodatapolygongroup_displaysettingwidget.h"

#include <geodata/polygon/geodatapolygon.h>
#include <geodata/polygon/geodatapolygonholepolygon.h>
#include <geodata/polygon/geodatapolygonregionpolygon.h>
#include <geodata/polygon/private/geodatapolygon_impl.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataitemi.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

#include <vtkAppendPolyData.h>

void GeoDataPolygonGroupProxy::Impl::setupActors(vtkActor* edgesActor, vtkPolyData* edgesData, vtkActor* paintActor, vtkPolyData* paintData, const GeoDataPolygonGroup::DisplaySetting& ds, ColorMapSettingContainerI* cm)
{
	// color
	QColor c = ds.color;

	edgesActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
	paintActor->GetProperty()->SetColor(ds.color);

	// opacity
	paintActor->GetProperty()->SetOpacity(ds.opacity);

	// mapping
	if (ds.mapping == GeoDataPolygonGroup::DisplaySetting::Mapping::Value && (cm != nullptr)) {
		vtkMapper* mapper = nullptr;

		mapper = cm->buildCellDataMapper(edgesData, true);
		edgesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = cm->buildCellDataMapper(paintData, false);
		paintActor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(edgesData);
		edgesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(paintData);
		paintActor->SetMapper(mapper);
		mapper->Delete();
	}

	// line width
	edgesActor->GetProperty()->SetLineWidth(ds.lineWidth);
}

GeoDataPolygonGroupProxy::GeoDataPolygonGroupProxy(GeoDataPolygonGroup* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{
	impl->m_displaySetting.displaySetting = geodata->impl->m_displaySetting;
}

GeoDataPolygonGroupProxy::~GeoDataPolygonGroupProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_paintActor);
	r->RemoveActor(impl->m_edgesActor);
	r->RemoveActor(impl->m_editTargetPaintActor);
	r->RemoveActor(impl->m_editTargetEdgesActor);
	delete impl;
}

void GeoDataPolygonGroupProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_paintActor);
	col->AddItem(impl->m_paintActor);

	r->AddActor(impl->m_edgesActor);
	col->AddItem(impl->m_edgesActor);

	// edit target actors are added to the actor collection in updateActorSetting(), only when edit target data exists
	r->AddActor(impl->m_editTargetPaintActor);
	impl->m_editTargetPaintActor->VisibilityOff();

	r->AddActor(impl->m_editTargetEdgesActor);
	impl->m_editTargetEdgesActor->VisibilityOff();

	updateActorSetting();
}

void GeoDataPolygonGroupProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPolygonGroupProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataPolygonGroupProxy::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoData()->gridAttribute()->name(), parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Polygons Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void GeoDataPolygonGroupProxy::updateActorSetting()
{
	auto polygons = dynamic_cast<GeoDataPolygonGroup*>(geoData());
	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = polygons->impl->m_displaySetting;
	}
	auto cm = colorMapSettingContainer();

	Impl::setupActors(impl->m_edgesActor, polygons->impl->m_edgesPolyData, impl->m_paintActor, polygons->impl->m_paintPolyData, ds, cm);

	// The polygon selected in pre-processor is removed from m_edgesPolyData and m_paintPolyData, and held as edit target data,
	// so it should be drawn separately.
	auto col = actorCollection();
	col->RemoveItem(impl->m_editTargetPaintActor);
	col->RemoveItem(impl->m_editTargetEdgesActor);
	impl->m_editTargetPaintActor->VisibilityOff();
	impl->m_editTargetEdgesActor->VisibilityOff();

	auto target = dynamic_cast<GeoDataPolygon*> (polygons->editTargetData());
	if (target != nullptr) {
		auto edges = impl->m_editTargetEdgesPolyData;
		edges->RemoveAllInputs();
		edges->AddInputData(target->impl->m_regionPolygon->edgesPolyData());
		for (auto hole : target->impl->m_holePolygons) {
			edges->AddInputData(hole->edgesPolyData());
		}
		edges->Update();

		Impl::setupActors(impl->m_editTargetEdgesActor, edges->GetOutput(), impl->m_editTargetPaintActor, target->impl->m_polyData, ds, cm);
		col->AddItem(impl->m_editTargetPaintActor);
		col->AddItem(impl->m_editTargetEdgesActor);
	}

	updateVisibilityWithoutRendering();
}

void GeoDataPolygonGroupProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_edgesActor->SetPosition(0, 0, range.max());
	impl->m_paintActor->SetPosition(0, 0, range.min());
	impl->m_editTargetEdgesActor->SetPosition(0, 0, range.max());
	impl->m_editTargetPaintActor->SetPosition(0, 0, range.min());
}

void GeoDataPolygonGroupProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataPolygonGroupProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}
