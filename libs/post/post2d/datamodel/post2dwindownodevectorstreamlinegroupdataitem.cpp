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
#include <QXmlStreamWriter>

#include <vtkLookupTable.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRungeKutta4.h>
#include <vtkStructuredGridGeometryFilter.h>

Post2dWindowNodeVectorStreamlineGroupDataItem::Setting::Setting() :
	CompositeContainer {&currentSolution, &regionMode},
	currentSolution {"solution"},
	regionMode {"regionMode", StructuredGridRegion::rmFull}
{}

Post2dWindowNodeVectorStreamlineGroupDataItem::Post2dWindowNodeVectorStreamlineGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Streamlines"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupClipper();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
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
		QString name = pd->GetArray(i)->GetName();
		Post2dWindowNodeVectorStreamlineDataItem* item = new Post2dWindowNodeVectorStreamlineDataItem(name, gt->solutionCaption(name), this);
		m_childItems.append(item);
	}
}

Post2dWindowNodeVectorStreamlineGroupDataItem::~Post2dWindowNodeVectorStreamlineGroupDataItem()
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

class Post2dWindowNodeVectorStreamlineGroupDataItem::SelectSolutionCommand : public QUndoCommand
{
public:
	SelectSolutionCommand(const QString& newsol, Post2dWindowNodeVectorStreamlineGroupDataItem* item) :
		QUndoCommand {Post2dWindowNodeVectorStreamlineGroupDataItem::tr("Streamline Physical Value Change")},
		m_newCurrentSolution {newsol},
		m_oldCurrentSolution {item->m_setting.currentSolution},
		m_item {item}
	{}
	void redo() {
		m_item->setCurrentSolution(m_newCurrentSolution);
		m_item->updateActorSettings();
	}
	void undo() {
		m_item->setCurrentSolution(m_oldCurrentSolution);
		m_item->updateActorSettings();
	}

private:
	QString m_newCurrentSolution;
	QString m_oldCurrentSolution;

	Post2dWindowNodeVectorStreamlineGroupDataItem* m_item;
};

void Post2dWindowNodeVectorStreamlineGroupDataItem::exclusivelyCheck(Post2dWindowNodeVectorStreamlineDataItem* item)
{
	if (m_isCommandExecuting) {return;}
	if (item->standardItem()->checkState() != Qt::Checked) {
		pushRenderCommand(new SelectSolutionCommand("", this), this, true);
	} else {
		pushRenderCommand(new SelectSolutionCommand(item->name(), this), this, true);
	}
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::informGridUpdate()
{
	updateActorSettings();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::updateActorSettings()
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

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (ps == nullptr) {return;}
	if (m_setting.currentSolution == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	setupActors();
	updateVisibilityWithoutRendering();
	assignActorZValues(m_zDepthRange);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::setupClipper()
{
	m_IBCClipper = vtkSmartPointer<vtkClipPolyData>::New();
	m_IBCClipper->SetValue(PostZoneDataContainer::IBCLimit);
	m_IBCClipper->InsideOutOff();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::assignActorZValues(const ZDepthRange& range)
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

void Post2dWindowNodeVectorStreamlineGroupDataItem::update()
{
	informGridUpdate();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::setCurrentSolution(const QString& currentSol)
{
	Post2dWindowNodeVectorStreamlineDataItem* current = nullptr;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post2dWindowNodeVectorStreamlineDataItem* tmpItem = dynamic_cast<Post2dWindowNodeVectorStreamlineDataItem*>(*it);
		if (tmpItem->name() == currentSol) {
			current = tmpItem;
		}
		tmpItem->standardItem()->setCheckState(Qt::Unchecked);
	}
	if (current != nullptr) {
		current->standardItem()->setCheckState(Qt::Checked);
	}
	m_setting.currentSolution = currentSol;
}

vtkPointSet* Post2dWindowNodeVectorStreamlineGroupDataItem::getRegion()
{
	vtkPointSet* ps = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer()->data();
	if (m_setting.regionMode == StructuredGridRegion::rmFull) {
		return ps;
	} else if (m_setting.regionMode == StructuredGridRegion::rmActive) {
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
	return nullptr;
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
	m_setting.load(node);
	updateActorSettings();
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowNodeVectorStreamlineGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
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

void Post2dWindowNodeVectorStreamlineGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
}
