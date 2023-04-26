#include "geodatapolygongroup.h"
#include "geodatapolygongroupproxy.h"
#include "private/geodatapolygongroup_impl.h"
#include "private/geodatapolygongroupproxy_displaysettingwidget.h"
#include "private/geodatapolygongroupproxy_impl.h"
#include "public/geodatapolygongroup_displaysettingwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataiteminterface.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

GeoDataPolygonGroupProxy::GeoDataPolygonGroupProxy(GeoDataPolygonGroup* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{}

GeoDataPolygonGroupProxy::~GeoDataPolygonGroupProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_paintActor);
	r->RemoveActor(impl->m_edgesActor);
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

	// color
	QColor c = ds.color;

	impl->m_edgesActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
	impl->m_paintActor->GetProperty()->SetColor(ds.color);

	// opacity
	impl->m_paintActor->GetProperty()->SetOpacity(ds.opacity);

	// mapping
	auto cm = colorMapSettingContainer();
	if (ds.mapping == GeoDataPolygonGroup::DisplaySetting::Mapping::Value && (cm != nullptr)) {
		vtkMapper* mapper = nullptr;

		mapper = cm->buildCellDataMapper(polygons->impl->m_edgesPolyData, true);
		impl->m_edgesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = cm->buildCellDataMapper(polygons->impl->m_paintPolyData, false);
		impl->m_paintActor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(polygons->impl->m_edgesPolyData);
		impl->m_edgesActor->SetMapper(mapper);
		mapper->Delete();

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(polygons->impl->m_paintPolyData);
		impl->m_paintActor->SetMapper(mapper);
		mapper->Delete();
	}

	// line width
	impl->m_edgesActor->GetProperty()->SetLineWidth(ds.lineWidth);

	updateVisibilityWithoutRendering();
}

void GeoDataPolygonGroupProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_edgesActor->SetPosition(0, 0, range.max());
	impl->m_paintActor->SetPosition(0, 0, range.min());
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
