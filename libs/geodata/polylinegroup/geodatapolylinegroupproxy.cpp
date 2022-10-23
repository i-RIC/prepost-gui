#include "geodatapolylinegroup.h"
#include "geodatapolylinegroupproxy.h"
#include "geodatapolylinegroupcolorsettingdialog.h"

#include "private/geodatapolylinegroup_impl.h"
#include "private/geodatapolylinegroupproxy_impl.h"
#include "private/geodatapolylinegroupproxy_setsettingcommand.h"

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
	auto lines = dynamic_cast<GeoDataPolyLineGroup*> (geoData());

	auto r = renderer();
	auto col = actorCollection();
	/*
	auto stcc = lines->scalarsToColorsContainer();

	auto mapper = dynamic_cast<vtkPolyDataMapper*> (impl->m_edgesActor->GetMapper());
	mapper->SetInputData(lines->impl->m_edgesPolyData);
	if (stcc != nullptr) {
		mapper->SetLookupTable(stcc->vtkObj());
		mapper->SetUseLookupTableScalarRange(true);
	}
	*/

	r->AddActor(impl->m_edgesActor);
	col->AddItem(impl->m_edgesActor);

	updateGraphics();
}

void GeoDataPolyLineGroupProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

QDialog* GeoDataPolyLineGroupProxy::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPolyLineGroupColorSettingDialog(parent);
	dialog->setSetting(impl->m_setting);

	return dialog;
}

void GeoDataPolyLineGroupProxy::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<GeoDataPolyLineGroupColorSettingDialog*> (propDialog);

	pushRenderCommand(new SetSettingCommand(dialog->setting(), this));
}

void GeoDataPolyLineGroupProxy::updateGraphics()
{
	auto cs = impl->m_setting;

	// color
	impl->m_edgesActor->GetProperty()->SetColor(cs.color);

	// mapping
	bool scalarVisibility = true;
	if (cs.mapping == GeoDataPolyLineGroupColorSettingDialog::Arbitrary) {
		scalarVisibility = false;
	}
	impl->m_edgesActor->GetMapper()->SetScalarVisibility(scalarVisibility);

	// opacity
	impl->m_edgesActor->GetProperty()->SetOpacity(cs.opacity);

	// pointSize
	impl->m_edgesActor->GetProperty()->SetLineWidth(cs.lineWidth);
}

void GeoDataPolyLineGroupProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_edgesActor->SetPosition(0, 0, range.min());
}

void GeoDataPolyLineGroupProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);

	updateGraphics();
}

void GeoDataPolyLineGroupProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}
