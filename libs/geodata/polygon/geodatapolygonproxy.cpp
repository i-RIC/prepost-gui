#include "geodatapolygonholepolygon.h"
#include "geodatapolygonregionpolygon.h"
#include "geodatapolygonproxy.h"
#include "private/geodatapolygon_impl.h"
#include "private/geodatapolygonproxy_displaysettingwidget.h"
#include "private/geodatapolygonproxy_impl.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataitemi.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

#include <vtkAppendPolyData.h>

GeoDataPolygonProxy::GeoDataPolygonProxy(GeoDataPolygon* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{}

GeoDataPolygonProxy::~GeoDataPolygonProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_paintActor);
	r->RemoveActor(impl->m_edgesActor);
	delete impl;
}

void GeoDataPolygonProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_paintActor);
	col->AddItem(impl->m_paintActor);

	r->AddActor(impl->m_edgesActor);
	col->AddItem(impl->m_edgesActor);

	updateActorSetting();
}

void GeoDataPolygonProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPolygonProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataPolygonProxy::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoData()->gridAttribute()->name(), parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Polygon Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void GeoDataPolygonProxy::updateActorSetting()
{
	auto polygon = dynamic_cast<GeoDataPolygon*>(geoData());
	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = polygon->impl->m_displaySetting;
	}

	impl->m_edgesPolyData->RemoveAllInputs();
	impl->m_edgesPolyData->AddInputData(polygon->impl->m_regionPolygon->edgesPolyData());
	for (auto hole : polygon->impl->m_holePolygons) {
		impl->m_edgesPolyData->AddInputData(hole->edgesPolyData());
	}
	impl->m_edgesPolyData->Update();

	// color
	QColor c = ds.color;

	impl->m_edgesActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
	impl->m_paintActor->GetProperty()->SetColor(ds.color);

	// opacity
	impl->m_paintActor->GetProperty()->SetOpacity(ds.opacity);

	// mapping
	auto cm = colorMapSettingContainer();
	if (ds.mapping == GeoDataPolygon::DisplaySetting::Mapping::Value && cm != nullptr) {
		vtkMapper* mapper = nullptr;

		mapper = cm->buildCellDataMapper(impl->m_edgesPolyData->GetOutput(), true);
		impl->m_edgesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = cm->buildCellDataMapper(polygon->impl->m_polyData, false);
		impl->m_paintActor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(impl->m_edgesPolyData->GetOutput());
		impl->m_edgesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(polygon->impl->m_polyData);
		impl->m_paintActor->SetMapper(mapper);
		mapper->Delete();
	}

	// line width
	impl->m_edgesActor->GetProperty()->SetLineWidth(ds.lineWidth);

	updateVisibilityWithoutRendering();
}

void GeoDataPolygonProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_edgesActor->SetPosition(0, 0, range.max());
	impl->m_paintActor->SetPosition(0, 0, range.min());
}

void GeoDataPolygonProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataPolygonProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}
