#include "geodatapolygongroup.h"
#include "geodatapolygongroupproxy.h"
#include "geodatapolygongroupcolorsettingdialog.h"

#include "private/geodatapolygongroup_impl.h"
#include "private/geodatapolygongroupproxy_impl.h"
#include "private/geodatapolygongroupproxy_setsettingcommand.h"

#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
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
	auto polygons = dynamic_cast<GeoDataPolygonGroup*> (geoData());

	auto r = renderer();
	auto col = actorCollection();

	auto stcc = polygons->scalarsToColorsContainer();

	auto mapper = dynamic_cast<vtkPolyDataMapper*> (impl->m_paintActor->GetMapper());
	mapper->SetInputData(polygons->impl->m_paintPolyData);
	if (stcc != nullptr) {
		mapper->SetLookupTable(stcc->vtkObj());
		mapper->SetUseLookupTableScalarRange(true);
	}

	r->AddActor(impl->m_paintActor);
	col->AddItem(impl->m_paintActor);

	mapper = dynamic_cast<vtkPolyDataMapper*> (impl->m_edgesActor->GetMapper());
	mapper->SetInputData(polygons->impl->m_edgesPolyData);
	if (stcc != nullptr) {
		mapper->SetLookupTable(stcc->vtkObj());
		mapper->SetUseLookupTableScalarRange(true);
	}

	r->AddActor(impl->m_edgesActor);
	col->AddItem(impl->m_edgesActor);

	updateGraphics();
}

void GeoDataPolygonGroupProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

QDialog* GeoDataPolygonGroupProxy::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPolygonGroupColorSettingDialog(parent);
	dialog->setSetting(impl->m_setting);

	return dialog;
}

void GeoDataPolygonGroupProxy::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<GeoDataPolygonGroupColorSettingDialog*> (propDialog);

	pushRenderCommand(new SetSettingCommand(dialog->setting(), this));
}

void GeoDataPolygonGroupProxy::updateGraphics()
{
	auto cs = impl->m_setting;

	// color
	impl->m_edgesActor->GetProperty()->SetColor(cs.color);

	// mapping
	bool scalarVisibility = true;
	if (cs.mapping == GeoDataPolygonGroupColorSettingDialog::Arbitrary) {
		scalarVisibility = false;
	}
	impl->m_paintActor->GetMapper()->SetScalarVisibility(scalarVisibility);
	impl->m_edgesActor->GetMapper()->SetScalarVisibility(scalarVisibility);

	// opacity
	impl->m_paintActor->GetProperty()->SetOpacity(cs.opacity);
	impl->m_edgesActor->GetProperty()->SetOpacity(cs.opacity);

	// pointSize
	impl->m_edgesActor->GetProperty()->SetLineWidth(cs.lineWidth);
}

void GeoDataPolygonGroupProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_edgesActor->SetPosition(0, 0, range.min());
}

void GeoDataPolygonGroupProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);

	updateGraphics();
}

void GeoDataPolygonGroupProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}
