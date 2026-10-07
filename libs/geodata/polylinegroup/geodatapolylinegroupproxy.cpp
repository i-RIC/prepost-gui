#include "geodatapolylinegroup.h"
#include "geodatapolylinegroupproxy.h"
#include "private/geodatapolylinegroup_impl.h"
#include "private/geodatapolylinegroupproxy_impl.h"
#include "private/geodatapolylinegroupproxy_displaysettingwidget.h"
#include "public/geodatapolylinegroup_displaysettingwidget.h"

#include <geodata/polyline/geodatapolyline.h>
#include <geodata/polyline/geodatapolylineimplpolyline.h>
#include <geodata/polyline/private/geodatapolyline_impl.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataitemi.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

void GeoDataPolyLineGroupProxy::Impl::setupEdgesActor(vtkActor* actor, vtkPolyData* data, const GeoDataPolyLineGroup::DisplaySetting& ds, ColorMapSettingContainerI* cm)
{
	// color
	QColor c = ds.color;

	actor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());

	// opacity
	actor->GetProperty()->SetOpacity(ds.opacity);

	// mapping
	if (ds.mapping == GeoDataPolyLineGroup::DisplaySetting::Mapping::Value && (cm != nullptr)) {
		vtkMapper* mapper = nullptr;

		mapper = cm->buildCellDataMapper(data, true);
		actor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(data);
		actor->SetMapper(mapper);
		mapper->Delete();
	}

	// line width
	actor->GetProperty()->SetLineWidth(ds.lineWidth);
}

GeoDataPolyLineGroupProxy::GeoDataPolyLineGroupProxy(GeoDataPolyLineGroup* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{
	impl->m_displaySetting.displaySetting = geodata->impl->m_displaySetting;
}

GeoDataPolyLineGroupProxy::~GeoDataPolyLineGroupProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_edgesActor);
	r->RemoveActor(impl->m_editTargetActor);
	delete impl;
}

void GeoDataPolyLineGroupProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_edgesActor);
	col->AddItem(impl->m_edgesActor);

	// m_editTargetActor is added to the actor collection in updateActorSetting(), only when edit target data exists
	r->AddActor(impl->m_editTargetActor);
	impl->m_editTargetActor->VisibilityOff();

	updateActorSetting();
}

void GeoDataPolyLineGroupProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPolyLineGroupProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataPolyLineGroupProxy::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoData()->gridAttribute()->name(), parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Lines Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void GeoDataPolyLineGroupProxy::updateActorSetting()
{
	auto lines = dynamic_cast<GeoDataPolyLineGroup*>(geoData());
	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = lines->impl->m_displaySetting;
	}

	auto cm = colorMapSettingContainer();

	Impl::setupEdgesActor(impl->m_edgesActor, lines->impl->m_edgesPolyData, ds, cm);

	// The line selected in pre-processor is removed from m_edgesPolyData and held as edit target data,
	// so it should be drawn separately.
	auto col = actorCollection();
	col->RemoveItem(impl->m_editTargetActor);
	impl->m_editTargetActor->VisibilityOff();

	auto target = dynamic_cast<GeoDataPolyLine*> (lines->editTargetData());
	if (target != nullptr) {
		Impl::setupEdgesActor(impl->m_editTargetActor, target->impl->m_polyLine->linePolyData(), ds, cm);
		col->AddItem(impl->m_editTargetActor);
	}

	updateVisibilityWithoutRendering();
}

void GeoDataPolyLineGroupProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_edgesActor->SetPosition(0, 0, range.min());
	impl->m_editTargetActor->SetPosition(0, 0, range.min());
}

void GeoDataPolyLineGroupProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataPolyLineGroupProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}
