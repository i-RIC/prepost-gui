#include "geodatagdal.h"
#include "geodatagdalproxy.h"
#include "private/geodatagdal_impl.h"
#include "private/geodatagdalproxy_impl.h"
#include "private/geodatagdalproxy_displaysettingwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataitemi.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

#include <vtkGeometryFilter.h>
#include <vtkUnstructuredGrid.h>

GeoDataGdalProxy::GeoDataGdalProxy(GeoDataGdal* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{
	impl->m_displaySetting.displaySetting = geodata->impl->m_displaySetting;
}

GeoDataGdalProxy::~GeoDataGdalProxy()
{
	auto r = renderer();
	r->RemoveActor(impl->m_actor);

	delete impl;
}

void GeoDataGdalProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_actor);
	col->AddItem(impl->m_actor);

	updateActorSetting();
}

void GeoDataGdalProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataGdalProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataGdalProxy::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoData()->gridAttribute()->name(), parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Raster Data Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void GeoDataGdalProxy::updateActorSetting()
{
	auto gdal = dynamic_cast<GeoDataGdal*> (geoData());
	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = gdal->impl->m_displaySetting;
	}

	// color
	impl->m_actor->GetProperty()->SetColor(ds.color);

	// opacity
	impl->m_actor->GetProperty()->SetOpacity(ds.opacity);

	// mapping
	auto cm = colorMapSettingContainer();
	if (ds.mapping == GeoDataGdal::DisplaySetting::Mapping::Value && (cm != nullptr)) {
		vtkMapper* mapper = nullptr;

		gdal->m_threshold->Update();
		mapper = cm->buildCellDataMapper(gdal->m_threshold->GetOutput(), false);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();

	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto geometry = vtkSmartPointer<vtkGeometryFilter>::New();
		geometry->SetInputConnection(gdal->m_threshold->GetOutputPort());
		mapper->SetInputConnection(geometry->GetOutputPort());
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
	}
	updateVisibilityWithoutRendering();
}

void GeoDataGdalProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.min());
}

void GeoDataGdalProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataGdalProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}
