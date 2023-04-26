#include "geodatanetcdf.h"
#include "geodatanetcdfproxy.h"
#include "private/geodatanetcdf_impl.h"
#include "private/geodatanetcdfproxy_impl.h"
#include "private/geodatanetcdfproxy_displaysettingwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataiteminterface.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

#include <vtkGeometryFilter.h>
#include <vtkUnstructuredGrid.h>

GeoDataNetcdfProxy::GeoDataNetcdfProxy(GeoDataNetcdf* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{}

GeoDataNetcdfProxy::~GeoDataNetcdfProxy()
{
	auto r = renderer();
	r->RemoveActor(impl->m_actor);

	delete impl;
}

void GeoDataNetcdfProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_actor);
	col->AddItem(impl->m_actor);

	updateActorSetting();
}

void GeoDataNetcdfProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataNetcdfProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataNetcdfProxy::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoData()->gridAttribute()->name(), parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Raster Data Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void GeoDataNetcdfProxy::updateActorSetting()
{
	auto netcdf = dynamic_cast<GeoDataNetcdf*> (geoData());
	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = netcdf->impl->m_displaySetting;
	}

	// color
	impl->m_actor->GetProperty()->SetColor(ds.color);

	// opacity
	impl->m_actor->GetProperty()->SetOpacity(ds.opacity);

	// mapping
	auto cm = colorMapSettingContainer();
	if (ds.mapping == GeoDataNetcdf::DisplaySetting::Mapping::Value && (cm != nullptr)) {
		vtkMapper* mapper = nullptr;

		netcdf->m_threshold->Update();
		mapper = cm->buildCellDataMapper(netcdf->m_threshold->GetOutput(), false);
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();

	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		auto geometry = vtkSmartPointer<vtkGeometryFilter>::New();
		geometry->SetInputConnection(netcdf->m_threshold->GetOutputPort());
		mapper->SetInputConnection(geometry->GetOutputPort());
		impl->m_actor->SetMapper(mapper);
		mapper->Delete();
	}
	updateVisibilityWithoutRendering();
}

void GeoDataNetcdfProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.min());
}

void GeoDataNetcdfProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataNetcdfProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}
