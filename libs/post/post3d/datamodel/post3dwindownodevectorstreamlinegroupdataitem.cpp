#include "../post3dwindowdatamodel.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodevectorstreamlinedataitem.h"
#include "post3dwindownodevectorstreamlinegroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

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
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setDefaultValues();
	setupClipper();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	vtkPointData* pd = cont->data()->GetPointData();
	int number = pd->GetNumberOfArrays();
	for (int i = 0; i < number; i++) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {continue;}
		if (tmparray->GetNumberOfComponents() == 1) {
			// scalar attribute.
			continue;
		}
		std::string name = pd->GetArray(i)->GetName();
		Post3dWindowNodeVectorStreamlineDataItem* item = new Post3dWindowNodeVectorStreamlineDataItem(name, gt->solutionCaption(name), this);
		m_childItems.append(item);
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

class Post3dWindowNodeVectorStreamlineGroupDataItem::SelectSolutionCommand : public QUndoCommand
{
public:
	SelectSolutionCommand(const std::string& newsol, Post3dWindowNodeVectorStreamlineGroupDataItem* item) :
		QUndoCommand {Post3dWindowNodeVectorStreamlineGroupDataItem::tr("Streamline Physical Value Change")},
		m_newSolution (newsol),
		m_oldSolution (item->m_currentSolution),
		m_item {item}
	{}
	void redo() override {
		applySetting(m_newSolution);
	}
	void undo() override {
		applySetting(m_oldSolution);
	}

private:
	void applySetting(const std::string& sol)
	{
		m_item->setCurrentSolution(sol);
		m_item->updateActorSettings();
	}
	std::string m_newSolution;
	std::string m_oldSolution;

	Post3dWindowNodeVectorStreamlineGroupDataItem* m_item;
};


void Post3dWindowNodeVectorStreamlineGroupDataItem::exclusivelyCheck(Post3dWindowNodeVectorStreamlineDataItem* item)
{
	if (m_isCommandExecuting) {return;}
	if (item->standardItem()->checkState() != Qt::Checked) {
		pushRenderCommand(new SelectSolutionCommand("", this), this, true);
	} else {
		pushRenderCommand(new SelectSolutionCommand(item->name(), this), this, true);
	}
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::setDefaultValues()
{
	m_currentSolution= "";
	m_regionMode = StructuredGridRegion::rmFull;
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
	if (m_currentSolution == "") {return;}
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

void Post3dWindowNodeVectorStreamlineGroupDataItem::setCurrentSolution(const std::string& currentSol)
{
	Post3dWindowNodeVectorStreamlineDataItem* current = nullptr;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post3dWindowNodeVectorStreamlineDataItem* tmpItem = dynamic_cast<Post3dWindowNodeVectorStreamlineDataItem*>(*it);
		if (tmpItem->name() == currentSol) {
			current = tmpItem;
		}
		tmpItem->standardItem()->setCheckState(Qt::Unchecked);
	}
	if (current != nullptr) {
		current->standardItem()->setCheckState(Qt::Checked);
	}
	m_currentSolution = currentSol;
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
	setCurrentSolution(iRIC::toStr(elem.attribute("solution")));
	m_regionMode = static_cast<StructuredGridRegion::RegionMode>(elem.attribute("regionMode").toInt());
}

void Post3dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", m_currentSolution.c_str());
	writer.writeAttribute("regionMode", QString::number(static_cast<int>(m_regionMode)));
}
