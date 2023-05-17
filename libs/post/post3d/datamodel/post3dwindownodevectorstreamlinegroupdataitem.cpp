#include "post3dwindownodevectorstreamlinedataitem.h"
#include "post3dwindownodevectorstreamlinegroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkRungeKutta4.h>

Post3dWindowNodeVectorStreamlineGroupDataItem::Setting::Setting() :
	CompositeContainer ({&target}),
	target {"solution"}
{}

Post3dWindowNodeVectorStreamlineGroupDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post3dWindowNodeVectorStreamlineGroupDataItem::Setting& Post3dWindowNodeVectorStreamlineGroupDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

Post3dWindowNodeVectorStreamlineGroupDataItem::Post3dWindowNodeVectorStreamlineGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Streamlines"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_setting {},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	auto gt = cont->gridType();
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->data()->GetPointData())) {
		auto item = new Post3dWindowNodeVectorStreamlineDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}
}

Post3dWindowNodeVectorStreamlineGroupDataItem::~Post3dWindowNodeVectorStreamlineGroupDataItem()
{
	clearActors();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Streamline Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::informGridUpdate()
{
	updateActorSetting();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::clearActors()
{
	for (auto actor : m_streamlineActors) {
		renderer()->RemoveActor(actor);
	}
	m_actorCollection->RemoveAllItems();
	m_streamlineActors.clear();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::updateActorSetting()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(iRIC::toStr(m_setting.target), m_childItems, true);

	clearActors();

	auto cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}

	auto ps = cont->data()->data();
	if (ps == nullptr) {return;}
	if (m_setting.target == "") {return;}

	auto pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupActors();
	applyZScale();
	updateVisibilityWithoutRendering();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::update()
{
	informGridUpdate();
}

std::string Post3dWindowNodeVectorStreamlineGroupDataItem::target() const
{
	return iRIC::toStr(m_setting.target);
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();
	updateActorSetting();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::innerUpdateZScale(double zscale)
{
	m_zScale = zscale;
	applyZScale();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::applyZScale()
{
	for (auto actor : m_streamlineActors) {
		actor->SetScale(1, 1, m_zScale);
	}
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::setupStreamTracer(vtkStreamTracer* tracer)
{
	tracer->SetIntegrationStepUnit(vtkStreamTracer::CELL_LENGTH_UNIT);
	tracer->SetMaximumIntegrationStep(0.5);
	tracer->SetMaximumPropagation(10000.);
	tracer->SetInitialIntegrationStep(0.5);

	vtkRungeKutta4* integ = vtkRungeKutta4::New();
	tracer->SetIntegrator(integ);
	integ->Delete();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	setTarget(m_setting.target);
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}
