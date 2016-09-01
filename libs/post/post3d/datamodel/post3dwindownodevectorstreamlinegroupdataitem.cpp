#include "../post3dwindowdatamodel.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodevectorstreamlinedataitem.h"
#include "post3dwindownodevectorstreamlinegroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRungeKutta4.h>
#include <vtkStructuredGridGeometryFilter.h>

Post3dWindowNodeVectorStreamlineGroupDataItem::Post3dWindowNodeVectorStreamlineGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Streamlines"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_target {},
	m_regionMode {StructuredGridRegion::rmFull},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupClipper();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->GetPointData())) {
		auto item = new Post3dWindowNodeVectorStreamlineDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}
}

Post3dWindowNodeVectorStreamlineGroupDataItem::~Post3dWindowNodeVectorStreamlineGroupDataItem()
{
	for (int i = 0; i < m_streamlineActors.count(); ++i) {
		renderer()->RemoveActor(m_streamlineActors[i]);
		m_streamlineActors[i]->Delete();
	}
	for (int i = 0; i < m_streamlineMappers.count(); ++i) {
		m_streamlineMappers[i]->Delete();
	}
	for (int i = 0; i < m_streamTracers.count(); ++i) {
		m_streamTracers[i]->Delete();
	}
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Streamline Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::informGridUpdate()
{
	updateActorSettings();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::updateActorSettings()
{
	for (int i = 0; i < m_streamlineActors.count(); ++i) {
		renderer()->RemoveActor(m_streamlineActors[i]);
		m_streamlineActors[i]->Delete();
		m_streamlineMappers[i]->Delete();
		m_streamTracers[i]->Delete();
	}
	m_actorCollection->RemoveAllItems();
	m_streamlineActors.clear();
	m_streamlineMappers.clear();
	m_streamTracers.clear();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (m_target == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupActors();
	applyZScale();
	updateVisibilityWithoutRendering();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::setupClipper()
{
	m_IBCClipper = vtkSmartPointer<vtkClipPolyData>::New();
	m_IBCClipper->SetValue(PostZoneDataContainer::IBCLimit);
	m_IBCClipper->InsideOutOff();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (m_streamlineActors.count() == 0) {return;}
	if (m_streamlineActors.count() == 1) {
		m_streamlineActors[0]->SetPosition(0, 0, range.max());
		return;
	}
	for (int i = 0; i < m_streamlineActors.count(); ++i) {
		double depth = range.min() + static_cast<double>(i) / (m_streamlineActors.count() - 1) * (range.max() - range.min());
		m_streamlineActors[i]->SetPosition(0, 0, depth);
	}
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::update()
{
	informGridUpdate();
}

std::string Post3dWindowNodeVectorStreamlineGroupDataItem::target() const
{
	return m_target;
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_target = target;
	updateActorSettings();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::innerUpdateZScale(double zscale)
{
	m_zScale = zscale;
	applyZScale();
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::applyZScale()
{
	for (int i = 0; i < m_streamlineActors.count(); ++i) {
		m_streamlineActors[i]->SetScale(1, 1, m_zScale);
	}
}

vtkPointSet* Post3dWindowNodeVectorStreamlineGroupDataItem::getRegion()
{
	vtkPointSet* ps = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer()->data();
	if (m_regionMode == StructuredGridRegion::rmFull) {
		return ps;
	} else if (m_regionMode == StructuredGridRegion::rmActive) {
		vtkSmartPointer<vtkStructuredGridGeometryFilter> geoFilter = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		geoFilter->SetInputData(ps);
		geoFilter->Update();
		ps->GetPointData()->SetActiveScalars(iRIC::toStr(PostZoneDataContainer::IBC).c_str());
		m_IBCClipper->SetInputConnection(geoFilter->GetOutputPort());
		m_IBCClipper->Update();
		m_regionClippedPolyData = vtkSmartPointer<vtkPolyData>::New();
		m_regionClippedPolyData->DeepCopy(m_IBCClipper->GetOutput());
		m_regionClippedPolyData->GetPointData()->SetActiveScalars("");
		return m_regionClippedPolyData;
	}
	return 0;
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
	QDomElement elem = node.toElement();
	setTarget(iRIC::toStr(elem.attribute("solution")));
	m_regionMode = static_cast<StructuredGridRegion::RegionMode>(elem.attribute("regionMode").toInt());
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", m_target.c_str());
	writer.writeAttribute("regionMode", QString::number(static_cast<int>(m_regionMode)));
}
