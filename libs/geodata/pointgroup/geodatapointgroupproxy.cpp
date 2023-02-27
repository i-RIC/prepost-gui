#include "geodatapointgroup.h"
#include "geodatapointgroupproxy.h"
#include "geodatapointgroupcolorsettingdialog.h"

#include "private/geodatapointgroup_impl.h"
#include "private/geodatapointgroupproxy_impl.h"
#include "private/geodatapointgroupproxy_setsettingcommand.h"

#include <misc/zdepthrange.h>

GeoDataPointGroupProxy::GeoDataPointGroupProxy(GeoDataPointGroup* geodata) :
	GeoDataProxy(geodata),
	impl {new Impl {}}
{}

GeoDataPointGroupProxy::~GeoDataPointGroupProxy()
{
	auto r = renderer();

	r->RemoveActor(impl->m_pointsActor);
	delete impl;
}

void GeoDataPointGroupProxy::setupActors()
{
	auto points = dynamic_cast<GeoDataPointGroup*> (geoData());

	auto r = renderer();
	auto col = actorCollection();

	/*
	auto stcc = points->scalarsToColorsContainer();

	auto mapper = dynamic_cast<vtkPolyDataMapper*> (impl->m_pointsActor->GetMapper());
	mapper->SetInputData(points->impl->m_pointsPolyData);
	if (stcc != nullptr) {
		mapper->SetLookupTable(stcc->vtkObj());
		mapper->SetUseLookupTableScalarRange(true);
	}
	*/
	r->AddActor(impl->m_pointsActor);
	col->AddItem(impl->m_pointsActor);

	updateGraphics();
}

void GeoDataPointGroupProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

QDialog* GeoDataPointGroupProxy::propertyDialog(QWidget* parent)
{
	auto dialog = new GeoDataPointGroupColorSettingDialog(parent);
	dialog->setSetting(impl->m_setting);

	return dialog;
}

void GeoDataPointGroupProxy::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<GeoDataPointGroupColorSettingDialog*> (propDialog);

	pushRenderCommand(new SetSettingCommand(dialog->setting(), this));
}

void GeoDataPointGroupProxy::updateGraphics()
{
	auto cs = impl->m_setting;

	// color
	impl->m_pointsActor->GetProperty()->SetColor(cs.color);

	// mapping
	bool scalarVisibility = true;
	if (cs.mapping == GeoDataPointGroupColorSettingDialog::Arbitrary) {
		scalarVisibility = false;
	}
	impl->m_pointsActor->GetMapper()->SetScalarVisibility(scalarVisibility);

	// opacity
	impl->m_pointsActor->GetProperty()->SetOpacity(cs.opacity);

	// pointSize
	impl->m_pointsActor->GetProperty()->SetPointSize(cs.pointSize);
}

void GeoDataPointGroupProxy::assignActorZValues(const ZDepthRange& range)
{
	impl->m_pointsActor->SetPosition(0, 0, range.min());
}

void GeoDataPointGroupProxy::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);

	updateGraphics();
}

void GeoDataPointGroupProxy::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}
