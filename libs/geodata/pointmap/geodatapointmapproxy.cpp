#include "geodatapointmap.h"
#include "geodatapointmapproxy.h"
#include "private/geodatapointmap_impl.h"
#include "private/geodatapointmapproxy_impl.h"
#include "private/geodatapointmapproxy_displaysettingwidget.h"
#include "public/geodatapointmap_displaysettingwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataiteminterface.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

GeoDataPointmapProxy::GeoDataPointmapProxy(GeoDataPointmap* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{
	impl->m_displaySetting.displaySetting = geodata->impl->m_displaySetting;
}

GeoDataPointmapProxy::~GeoDataPointmapProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_tinActor);
	delete impl;
}

void GeoDataPointmapProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_tinActor);
	col->AddItem(impl->m_tinActor);

	updateActorSetting();
}

void GeoDataPointmapProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPointmapProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataPointmapProxy::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoData()->gridAttribute()->name(), parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Point Cloud Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void GeoDataPointmapProxy::updateActorSetting()
{
	auto pointmap = dynamic_cast<GeoDataPointmap*>(geoData());
	auto v = dataModel()->graphicsView();

	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = pointmap->impl->m_displaySetting;
	}

	auto cm = colorMapSettingContainer();
	vtkMapper* mapper = nullptr;
	if (cm != nullptr) {
		mapper = cm->buildPointDataMapper(pointmap->impl->m_tinManager.tin());
	} else {
		auto polyDataMapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		polyDataMapper->SetInputData(pointmap->impl->m_tinManager.tin());
		mapper = polyDataMapper;
	}
	auto actor = impl->m_tinActor;
	actor->SetMapper(mapper);
	mapper->Delete();

	actor->GetProperty()->SetPointSize(ds.tinPointSize * v->devicePixelRatioF());
	actor->GetProperty()->SetLineWidth(ds.tinLineWidth * v->devicePixelRatioF());
	actor->GetProperty()->SetOpacity(ds.tinOpacity);

	switch (ds.tinRepresentation.value()) {
	case GeoDataPointmap::DisplaySetting::TinRepresentation::Points:
		actor->GetProperty()->SetRepresentationToPoints();
		break;
	case GeoDataPointmap::DisplaySetting::TinRepresentation::Wireframe:
		actor->GetProperty()->SetRepresentationToWireframe();
		break;
	case GeoDataPointmap::DisplaySetting::TinRepresentation::Surface:
		actor->GetProperty()->SetRepresentationToSurface();
		break;
	}
	updateVisibilityWithoutRendering();
}

void GeoDataPointmapProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_tinActor->SetPosition(0, 0, range.min());
}

void GeoDataPointmapProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataPointmapProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}
