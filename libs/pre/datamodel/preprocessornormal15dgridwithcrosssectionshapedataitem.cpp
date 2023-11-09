#include "../subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindow.h"
#include "../subwindow/structured15dgridwithcrosssectioncrosssectionwindow/structured15dgridwithcrosssectioncrosssectionwindowprojectdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessornormal15dgridwithcrosssectiondataitem.h"
#include "preprocessornormal15dgridwithcrosssectionshapedataitem.h"
#include "public/preprocessorgriddataitem_selectednodescontroller.h"

#include <guicore/base/iricmainwindowi.h>
#include <guicore/grid/v4structured15dgridwithcrosssection.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/project/projectdata.h>
#include <misc/mathsupport.h>

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMouseEvent>

#include <vtkCell.h>
#include <vtkCollectionIterator.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

PreProcessorNormal15DGridWithCrossSectionShapeDataItem::PreProcessorNormal15DGridWithCrossSectionShapeDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridShapeDataItem {parent}
{
	m_openCrossSectionWindowAction = new QAction(PreProcessorNormal15DGridWithCrossSectionShapeDataItem::tr("Display &Cross Section"), this);
	connect(m_openCrossSectionWindowAction, SIGNAL(triggered()), this, SLOT(openCrossSectionWindow()));

	setupActors();
	updateActorSettings();
}

PreProcessorNormal15DGridWithCrossSectionShapeDataItem::~PreProcessorNormal15DGridWithCrossSectionShapeDataItem()
{
	renderer()->RemoveActor(m_edgeActor);
	renderer()->RemoveActor(m_vertexActor);
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::setupActors()
{
	auto v = dataModel()->graphicsView();

	m_edgeActor = vtkSmartPointer<vtkActor>::New();
	m_edgeActor->GetProperty()->SetLighting(false);
	m_edgeActor->GetProperty()->SetLineWidth(normalLineWidth * v->devicePixelRatioF());
	renderer()->AddActor(m_edgeActor);

	m_edgeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_edgeActor->SetMapper(m_edgeMapper);

	m_vertexActor = vtkSmartPointer<vtkActor>::New();
	m_vertexActor->GetProperty()->SetLighting(false);
	m_vertexActor->GetProperty()->SetPointSize(normalPointSize * v->devicePixelRatioF());
	renderer()->AddActor(m_vertexActor);

	m_vertexMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_vertexActor->SetMapper(m_vertexMapper);

	m_edgeActor->VisibilityOff();
	m_vertexActor->VisibilityOff();
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::informGridUpdate()
{
	v4InputGrid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g != nullptr) {
		auto grid = dynamic_cast<v4Structured15dGridWithCrossSection*> (g->grid());

		m_edgeMapper->SetInputData(grid->vtkData()->data());
		m_vertexMapper->SetInputData(grid->pointsGrid());
	}
	updateActorSettings();
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::updateActorSettings()
{
	vtkCollectionIterator* it = m_actorCollection->NewIterator();
	it->GoToFirstItem();
	while (! it->IsDoneWithTraversal()) {
		vtkActor* actor = vtkActor::SafeDownCast(it->GetCurrentObject());
		actor->VisibilityOff();
		it->GoToNextItem();
	}
	m_actorCollection->RemoveAllItems();

	v4InputGrid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g == nullptr) {
		return;
	}

	m_edgeActor->GetProperty()->SetColor(m_setting.color);
	m_actorCollection->AddItem(m_edgeActor);
	m_vertexActor->GetProperty()->SetColor(m_setting.color);
	m_actorCollection->AddItem(m_vertexActor);

	updateVisibility();
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_edgeActor->GetProperty()->SetLineWidth(selectedLineWidth);
	m_vertexActor->GetProperty()->SetPointSize(selectedPointSize);
	gridDataItem()->selectedNodesController()->setVisibility(true);
	updateVisibility();
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_edgeActor->GetProperty()->SetLineWidth(normalLineWidth);
	m_vertexActor->GetProperty()->SetPointSize(normalPointSize);
	gridDataItem()->selectedNodesController()->setVisibility(false);
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_edgeActor->SetPosition(0, 0, range.min());
	m_vertexActor->SetPosition(0, 0, range.max());
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::addCustomMenuItems(QMenu* menu)
{
	PreProcessorGridShapeDataItem::addCustomMenuItems(menu);
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::updateActionStatus()
{
	PreProcessorGridShapeDataItem::updateActionStatus();

	m_openCrossSectionWindowAction->setEnabled(gridDataItem()->selectedNodesController()->selectedDataIds().size() > 0);
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::openCrossSectionWindow()
{
	auto i = new Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem(this, preProcessorWindow());
	i->window()->setTarget(selectedCrossSection());
	m_crosssectionWindows.append(i);
	QMdiArea* cent = dynamic_cast<QMdiArea*>(iricMainWindow()->centralWidget());
	QMdiSubWindow* container = cent->addSubWindow(i->window());
	container->show();
	container->setFocus();
	i->window()->cameraFit();
}

v4Structured15dGridWithCrossSectionCrossSection* PreProcessorNormal15DGridWithCrossSectionShapeDataItem::selectedCrossSection()
{
	auto gItem = gridDataItem();
	auto g = dynamic_cast<v4Structured15dGridWithCrossSection*>(gItem->grid()->grid());

	if (gItem->selectedNodesController()->selectedDataIds().size() < 1) {return nullptr;}

	vtkIdType index = gItem->selectedNodesController()->selectedDataIds().at(0);
	auto it = g->crossSections().begin();
	return *(it + index);
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::updateCrossSectionWindows()
{
	for (auto w : m_crosssectionWindows) {
		auto w2 = w->window();
		bool ok = w2->updateComboBoxes();
		if (ok) {
			w2->setupData();
			w2->updateView();
		}
	}
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::requestCrossSectionWindowDelete(Structured15DGridWithCrossSectionCrossSectionWindowProjectDataItem* item)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		if (*it == item) {
			m_crosssectionWindows.erase(it);
			delete item;
			return;
		}
	}
}

void PreProcessorNormal15DGridWithCrossSectionShapeDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	static QMenu* menu = nullptr;
	auto gItem = gridDataItem();
	if (event->button() == Qt::LeftButton) {
		if (m_definingBoundingBox) {
			gItem->selectedNodesController()->handleMouseReleaseEvent(event, v);
		}
		m_definingBoundingBox = false;
		m_draggingSelectedPoints = false;
	} else if (event->button() == Qt::RightButton) {
		QPoint releasePoint(event->x(), event->y());
		if (iRIC::isNear(m_pressPoint, releasePoint)) {
			delete menu;
			menu = new QMenu(projectData()->mainWindow());
			menu->addAction(m_editAction);
			menu->addAction(m_openCrossSectionWindowAction);
			menu->move(event->globalPos());
			menu->show();
		}
	}
}

