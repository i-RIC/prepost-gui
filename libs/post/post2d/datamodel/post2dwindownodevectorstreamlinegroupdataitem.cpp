#include "../post2dwindowdatamodel.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodevectorstreamlinedataitem.h"
#include "post2dwindownodevectorstreamlinegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QMenu>
#include <QMouseEvent>
#include <QSettings>
#include <QStandardItem>
#include <QUndoCommand>

#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRungeKutta4.h>
#include <vtkStructuredGridGeometryFilter.h>

Post2dWindowNodeVectorStreamlineGroupDataItem::Post2dWindowNodeVectorStreamlineGroupDataItem(Post2dWindowDataItem* p)
	: Post2dWindowDataItem(tr("Streamlines"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	setDefaultValues();
	setupClipper();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	vtkPointData* pd = cont->data()->GetPointData();
	int number = pd->GetNumberOfArrays();
	for (int i = 0; i < number; i++){
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == 0){continue;}
		if (tmparray->GetNumberOfComponents() == 1){
			// scalar attribute.
			continue;
		}
		QString name = pd->GetArray(i)->GetName();
		Post2dWindowNodeVectorStreamlineDataItem* item = new Post2dWindowNodeVectorStreamlineDataItem(name, gt->solutionCaption(name), this);
		m_childItems.append(item);
	}
}

Post2dWindowNodeVectorStreamlineGroupDataItem::~Post2dWindowNodeVectorStreamlineGroupDataItem()
{
	for (int i = 0; i < m_streamlineActors.count(); ++i){
		renderer()->RemoveActor(m_streamlineActors[i]);
		m_streamlineActors[i]->Delete();
	}
	for (int i = 0; i < m_streamlineMappers.count(); ++i){
		m_streamlineMappers[i]->Delete();
	}
	for (int i = 0; i < m_streamTracers.count(); ++i){
		m_streamTracers[i]->Delete();
	}
}

class Post2dWindowStructuredGridStreamlineSelectSolution : public QUndoCommand
{
public:
	Post2dWindowStructuredGridStreamlineSelectSolution(const QString& newsol, Post2dWindowNodeVectorStreamlineGroupDataItem* item)
		: QUndoCommand(QObject::tr("Streamline Physical Value Change"))
	{
		m_newCurrentSolution = newsol;
		m_oldCurrentSolution = item->m_currentSolution;
		m_item = item;
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_oldCurrentSolution);
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentSolution(m_newCurrentSolution);
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	QString m_oldCurrentSolution;
	QString m_newCurrentSolution;

	Post2dWindowNodeVectorStreamlineGroupDataItem* m_item;
};


void Post2dWindowNodeVectorStreamlineGroupDataItem::exclusivelyCheck(Post2dWindowNodeVectorStreamlineDataItem* item)
{
	if (m_isCommandExecuting){return;}
	iRICUndoStack& stack = iRICUndoStack::instance();
	if (item->standardItem()->checkState() != Qt::Checked){
		stack.push(new Post2dWindowStructuredGridStreamlineSelectSolution("", this));
	}else{
		stack.push(new Post2dWindowStructuredGridStreamlineSelectSolution(item->name(), this));
	}
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::setDefaultValues()
{
	m_currentSolution= "";
	m_regionMode = StructuredGridRegion::rmFull;
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::informGridUpdate()
{
	updateActorSettings();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::updateActorSettings()
{
	for (int i = 0; i < m_streamlineActors.count(); ++i){
		renderer()->RemoveActor(m_streamlineActors[i]);
		m_streamlineActors[i]->Delete();
		m_streamlineMappers[i]->Delete();
		m_streamTracers[i]->Delete();
	}
	m_actorCollection->RemoveAllItems();
	m_streamlineActors.clear();
	m_streamlineMappers.clear();
	m_streamTracers.clear();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == 0){return;}
	vtkPointSet* ps = cont->data();
	if (ps == 0){return;}
	if (m_currentSolution == ""){return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0){return;}

	setupActors();
	updateVisibilityWithoutRendering();
	assignActionZValues(m_zDepthRange);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::setupClipper(){
	m_IBCClipper = vtkSmartPointer<vtkClipPolyData>::New();
	m_IBCClipper->SetValue(PostZoneDataContainer::IBCLimit);
	m_IBCClipper->InsideOutOff();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::assignActionZValues(const ZDepthRange& range)
{
	if (m_streamlineActors.count() == 0){return;}
	if (m_streamlineActors.count() == 1){
		m_streamlineActors[0]->SetPosition(0, 0, range.max());
		return;
	}
	for (int i = 0; i < m_streamlineActors.count(); ++i){
		double depth = range.min() + static_cast<double>(i) / (m_streamlineActors.count() - 1) * (range.max() - range.min());
		m_streamlineActors[i]->SetPosition(0, 0, depth);
	}
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::update()
{
	informGridUpdate();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::setCurrentSolution(const QString& currentSol)
{
	QList<GraphicsWindowDataItem*>::iterator it;
	Post2dWindowNodeVectorStreamlineDataItem* current = 0;
	for (it = m_childItems.begin(); it != m_childItems.end(); ++it){
		Post2dWindowNodeVectorStreamlineDataItem* tmpItem = dynamic_cast<Post2dWindowNodeVectorStreamlineDataItem*>(*it);
		if (tmpItem->name() == currentSol){
			current = tmpItem;
		}
		tmpItem->standardItem()->setCheckState(Qt::Unchecked);
	}
	if (current != 0){
		current->standardItem()->setCheckState(Qt::Checked);
	}
	m_currentSolution = currentSol;
}

vtkPointSet* Post2dWindowNodeVectorStreamlineGroupDataItem::getRegion()
{
	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->data();
	if (m_regionMode == StructuredGridRegion::rmFull){
		return ps;
	} else if (m_regionMode == StructuredGridRegion::rmActive){
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
	// only to avoid warning.
	return 0;
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

void Post2dWindowNodeVectorStreamlineGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	setCurrentSolution(elem.attribute("solution"));
	m_regionMode = static_cast<StructuredGridRegion::RegionMode>(elem.attribute("regionMode").toInt());
	updateActorSettings();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", m_currentSolution);
	writer.writeAttribute("regionMode", QString::number(static_cast<int>(m_regionMode)));
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::informSelection(VTKGraphicsView * /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeAttributeBrowser();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::addCustomMenuItems(QMenu *menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
}
