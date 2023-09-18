#include "geodatapolylineimplpolyline.h"
#include "geodatapolylineproxy.h"
#include "private/geodatapolyline_impl.h"
#include "private/geodatapolylineproxy_displaysettingwidget.h"
#include "private/geodatapolylineproxy_impl.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataitemi.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

#include <vtkPolyDataMapper.h>

GeoDataPolyLineProxy::GeoDataPolyLineProxy(GeoDataPolyLine* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{}

GeoDataPolyLineProxy::~GeoDataPolyLineProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_edgesActor);
	delete impl;
}

void GeoDataPolyLineProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_edgesActor);
	col->AddItem(impl->m_edgesActor);

	updateActorSetting();
}

void GeoDataPolyLineProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GeoDataPolyLineProxy::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* GeoDataPolyLineProxy::propertyDialog(QWidget* parent)
{
	auto dialog = gridTypeDataItem()->createApplyColorMapSettingDialog(geoData()->gridAttribute()->name(), parent);
	auto widget = new DisplaySettingWidget(this, dialog);
	dialog->setWidget(widget);
	dialog->setWindowTitle(tr("Line Display Setting"));
	dialog->resize(900, 700);

	return dialog;
}

void GeoDataPolyLineProxy::updateActorSetting()
{
	auto line = dynamic_cast<GeoDataPolyLine*>(geoData());
	auto ds = impl->m_displaySetting.displaySetting;
	if (impl->m_displaySetting.usePreSetting) {
		ds = line->impl->m_displaySetting;
	}

	// color
	QColor c = ds.color;

	impl->m_edgesActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());

	// opacity
	impl->m_edgesActor->GetProperty()->SetOpacity(ds.opacity);

	// mapping
	auto cm = colorMapSettingContainer();
	if (ds.mapping == GeoDataPolyLine::DisplaySetting::Mapping::Value && cm != nullptr) {
		vtkMapper* mapper = nullptr;

		mapper = cm->buildCellDataMapper(line->impl->m_polyLine->linePolyData(), true);
		impl->m_edgesActor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(line->impl->m_polyLine->linePolyData());
		impl->m_edgesActor->SetMapper(mapper);
		mapper->Delete();
	}

	// line width
	impl->m_edgesActor->GetProperty()->SetLineWidth(ds.lineWidth);
	updateVisibilityWithoutRendering();
}

void GeoDataPolyLineProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_edgesActor->SetPosition(0, 0, range.max());
}

void GeoDataPolyLineProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_displaySetting.load(node);

	updateActorSetting();
}

void GeoDataPolyLineProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_displaySetting.save(writer);
}
