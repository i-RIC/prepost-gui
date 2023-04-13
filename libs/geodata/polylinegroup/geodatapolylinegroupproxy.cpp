#include "geodatapolylinegroup.h"
#include "geodatapolylinegroupproxy.h"
#include "private/geodatapolylinegroup_impl.h"
#include "private/geodatapolylinegroupproxy_impl.h"
#include "private/geodatapolylinegroupproxy_displaysettingwidget.h"
#include "public/geodatapolylinegroup_displaysettingwidget.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/post/post2d/base/post2dwindowgridtypedataiteminterface.h>
#include <misc/modifycommanddialog.h>
#include <misc/zdepthrange.h>

GeoDataPolyLineGroupProxy::GeoDataPolyLineGroupProxy(GeoDataPolyLineGroup* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{}

GeoDataPolyLineGroupProxy::~GeoDataPolyLineGroupProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_edgesActor);
	delete impl;
}

void GeoDataPolyLineGroupProxy::setupActors()
{
	auto r = renderer();
	auto col = actorCollection();

	r->AddActor(impl->m_edgesActor);
	col->AddItem(impl->m_edgesActor);

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

	// color
	QColor c = ds.color;

	impl->m_edgesActor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());

	// opacity
	impl->m_edgesActor->GetProperty()->SetOpacity(ds.opacity);

	// mapping
	auto cm = colorMapSettingContainer();
	if (ds.mapping == GeoDataPolyLineGroup::DisplaySetting::Mapping::Value && (cm != nullptr)) {
		vtkMapper* mapper = nullptr;

		mapper = cm->buildCellDataMapper(lines->impl->m_edgesPolyData, true);
		impl->m_edgesActor->SetMapper(mapper);
		mapper->Delete();
	} else {
		vtkPolyDataMapper* mapper = nullptr;

		mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(lines->impl->m_edgesPolyData);
		impl->m_edgesActor->SetMapper(mapper);
		mapper->Delete();
	}

	// line width
	impl->m_edgesActor->GetProperty()->SetLineWidth(ds.lineWidth);
}

void GeoDataPolyLineGroupProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_edgesActor->SetPosition(0, 0, range.min());
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
