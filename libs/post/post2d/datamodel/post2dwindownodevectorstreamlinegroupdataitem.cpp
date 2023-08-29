#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodevectorstreamlinedataitem.h"
#include "post2dwindownodevectorstreamlinegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QMenu>
#include <QMouseEvent>

#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkRungeKutta4.h>

Post2dWindowNodeVectorStreamlineGroupDataItem::Setting::Setting() :
	CompositeContainer ({&target}),
	target {"solution"}
{}

Post2dWindowNodeVectorStreamlineGroupDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowNodeVectorStreamlineGroupDataItem::Setting& Post2dWindowNodeVectorStreamlineGroupDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

Post2dWindowNodeVectorStreamlineGroupDataItem::Post2dWindowNodeVectorStreamlineGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Streamlines"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->data()->GetPointData())) {
		auto caption = gt->vectorOutputCaption(name);
		auto item = new Post2dWindowNodeVectorStreamlineDataItem(name, caption, this);
		m_childItems.push_back(item);
	}
}

Post2dWindowNodeVectorStreamlineGroupDataItem::~Post2dWindowNodeVectorStreamlineGroupDataItem()
{
	clearActors();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Streamline Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::informGridUpdate()
{
	updateActorSetting();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::updateActorSetting()
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(iRIC::toStr(m_setting.target), m_childItems, true);

	clearActors();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}

	vtkPointSet* ps = cont->data()->data();
	if (ps == nullptr) {return;}
	if (m_setting.target == "") {return;}

	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupActors();
	updateVisibilityWithoutRendering();
	assignActorZValues(m_zDepthRange);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (m_streamlineActors.size() == 0) {return;}
	if (m_streamlineActors.size() == 1) {
		m_streamlineActors[0]->SetPosition(0, 0, range.max());
		return;
	}
	for (int i = 0; i < static_cast<int> (m_streamlineActors.size()); ++i) {
		double depth = range.min() + static_cast<double>(i) / (m_streamlineActors.size() - 1) * (range.max() - range.min());
		m_streamlineActors[i]->SetPosition(0, 0, depth);
	}
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::update()
{
	informGridUpdate();
}

std::string Post2dWindowNodeVectorStreamlineGroupDataItem::target() const
{
	return m_setting.target;
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();
	updateActorSetting();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::setupStreamTracer(vtkStreamTracer* tracer)
{
	tracer->SetIntegrationStepUnit(vtkStreamTracer::CELL_LENGTH_UNIT);
	tracer->SetMaximumIntegrationStep(0.5);
	tracer->SetMaximumPropagation(10000.);
	tracer->SetInitialIntegrationStep(0.5);

	vtkRungeKutta4* integ = vtkRungeKutta4::New();
	tracer->SetIntegrator(integ);
	integ->Delete();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	setTarget(m_setting.target);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;

	for (auto actor : m_streamlineActors) {
		actor->SetScale(1, scale, 1);
	}
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::clearActors()
{
	for (auto actor : m_streamlineActors) {
		renderer()->RemoveActor(actor);
	}
	m_actorCollection->RemoveAllItems();
	m_streamlineActors.clear();
}
