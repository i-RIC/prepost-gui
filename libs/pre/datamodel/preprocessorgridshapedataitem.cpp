#include "../preprocessordatamodel.h"
#include "../preprocessorwindow.h"
#include "../subwindow/gridcrosssectionwindow/preprocessorgridcrosssectionwindow.h"
#include "preprocessorgridattributenodedataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem.h"
#include "private/preprocessorgridshapedataitem_updatepointscommand.h"
#include "private/preprocessorgridshapedataitem_positiondeltadialog.h"
#include "private/preprocessorgridshapedataitem_positioneditdialog.h"
#include "public/preprocessorgriddataitem_selectednodescontroller.h"

#include <guibase/widget/coloreditwidget.h>
#include <guibase/graphicsmisc.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/project/projectdata.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <algorithm>

PreProcessorGridShapeDataItem::PreProcessorGridShapeDataItem(PreProcessorDataItem* parent) :
	PreProcessorDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	m_definingBoundingBox {false},
	m_draggingSelectedPoints {false},
	m_nearSelectedPoint {false}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	parent->standardItem()->takeRow(m_standardItem->row());

	m_setting.shape = GridShapeSettingContainer::Shape::Wireframe;

	m_editAction = new QAction(PreProcessorGridShapeDataItem::tr("&Edit Coordinates..."), this);
	connect(m_editAction, SIGNAL(triggered()), this, SLOT(editShape()));

	m_openXsectionWindowAction = new QAction(PreProcessorGridShapeDataItem::tr("Open &Cross Section Window"), this);
	m_openXsectionWindowAction->setDisabled(true);
	connect(m_openXsectionWindowAction, SIGNAL(triggered()), this, SLOT(openCrossSectionWindow()));

	m_openVXsectionWindowAction = new QAction(PreProcessorGridShapeDataItem::tr("Open &Longitudinal Cross Section Window"), this);
	m_openVXsectionWindowAction->setDisabled(true);
	connect(m_openVXsectionWindowAction, SIGNAL(triggered()), this, SLOT(openVerticalCrossSectionWindow()));
}

PreProcessorGridShapeDataItem::~PreProcessorGridShapeDataItem()
{}

void PreProcessorGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

void PreProcessorGridShapeDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto v2 = dynamic_cast<PreProcessorGraphicsViewI*>(v);
	if (m_draggingSelectedPoints) {
		auto gItem = gridDataItem();
		auto newpoints = vtkSmartPointer<vtkPoints>::New();
		newpoints->DeepCopy(m_pointsBeforeDragging);
		auto dragEnd = v2->viewportToWorld(event->pos());
		auto delta = dragEnd - m_dragStartPoint;
		const auto& selVertices = gItem->selectedNodesController()->selectedDataIds();
		for (auto vid : selVertices) {
			double x[3];
			m_pointsBeforeDragging->GetPoint(vid, x);
			x[0] += delta.x();
			x[1] += delta.y();
			newpoints->SetPoint(vid, x);
		}
		newpoints->Modified();
		pushRenderCommand(new UpdatePointsCommand(false, newpoints, gItem), this);
	} else if (m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		gridDataItem()->selectedNodesController()->handleMouseMoveEvent(event, v);
	} else {
		// free mouse cursor move.
		// ingestigate whether there exists a selected point
		// near enough to the mouse pointer.
		auto gItem = gridDataItem();

		if (gItem->selectedNodesController()->selectedDataIds().size() == 0) {return;}
		auto grid2d = dynamic_cast<v4Grid2d*> (gItem->grid()->grid());
		if (grid2d->isMasked()) {
			v->setCursor(Qt::ForbiddenCursor);
			return;
		} else {
			v->setCursor(Qt::ArrowCursor);
		}

		auto worldPos = v2->viewportToWorld(event->pos());
		double p[3];
		p[0] = worldPos.x();
		p[1] = worldPos.y();
		p[2] = 0;
		double radius = v2->stdRadius(iRIC::nearRadius());
		double dist2;

		auto locator = gItem->grid()->grid()->vtkData()->pointLocator();
		vtkIdType closestId = locator->FindClosestPointWithinRadius(radius, p, dist2);
		if (closestId >= 0) {
			const auto& selectedVerts = gItem->selectedNodesController()->selectedDataIds();
			m_nearSelectedPoint = std::binary_search(selectedVerts.begin(), selectedVerts.end(), closestId);
		} else {
			m_nearSelectedPoint = false;
		}
		if (m_nearSelectedPoint) {
			v->setCursor(Qt::OpenHandCursor);
		} else {
			v->setCursor(gItem->normalCursor());
		}
	}
}

void PreProcessorGridShapeDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	PreProcessorGraphicsViewI* v2 = dynamic_cast<PreProcessorGraphicsViewI*>(v);
	if (event->button() == Qt::LeftButton) {
		if (m_nearSelectedPoint) {
			iRICMainWindowI* mw = dataModel()->iricMainWindow();
			if (mw->isSolverRunning()) {
				mw->warnSolverRunning();
				return;
			}
			auto tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
			// start dragging of the selected points.
			m_draggingSelectedPoints = true;
			m_pointsBeforeDragging = vtkSmartPointer<vtkPoints>::New();
			m_pointsBeforeDragging->DeepCopy(tmpparent->grid()->grid()->vtkData()->data()->GetPoints());
			m_dragStartPoint = v2->viewportToWorld(event->pos());
			v->setCursor(Qt::ClosedHandCursor);
		} else {
			// start drawing the mouse bounding box.
			m_definingBoundingBox = true;
			gridDataItem()->selectedNodesController()->handleMousePressEvent(event, v);
		}
	}
	m_pressPoint = event->pos();
}

void PreProcessorGridShapeDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	static QMenu* menu = nullptr;
	auto gItem = gridDataItem();
	if (event->button() == Qt::LeftButton) {
		if (m_draggingSelectedPoints) {
			auto newPoints = gItem->grid()->grid()->vtkData()->data()->GetPoints();
			newPoints->Register(nullptr);
			pushRenderCommand(new UpdatePointsCommand(true, newPoints, gItem), this);
		} else if (m_definingBoundingBox) {
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

			auto grid2d = dynamic_cast<v4Structured2dGrid*>(gItem->grid()->grid());
			if (grid2d != nullptr) {
				menu->addSeparator();

				menu->addAction(m_openXsectionWindowAction);
				menu->addAction(m_openVXsectionWindowAction);
			}

			menu->move(event->globalPos());
			menu->show();
		}
	}
}

void PreProcessorGridShapeDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	gridDataItem()->selectedNodesController()->handleKeyPressEvent(event, v);
}

void PreProcessorGridShapeDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	gridDataItem()->selectedNodesController()->handleKeyReleaseEvent(event, v);
}

PreProcessorGridDataItem* PreProcessorGridShapeDataItem::gridDataItem() const
{
	return dynamic_cast<PreProcessorGridDataItem*> (parent());
}

void PreProcessorGridShapeDataItem::editShape()
{
	auto mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}

	auto gItem = gridDataItem();
	const auto& selVertices = gItem->selectedNodesController()->selectedDataIds();

	if (selVertices.size() == 1) {
		// only one node is selected.
		PositionEditDialog dialog(preProcessorWindow());
		vtkIdType pid = selVertices.at(0);
		double x[3];
		gItem->grid()->grid()->vtkData()->data()->GetPoint(pid, x);
		dialog.setPosition(x[0], x[1]);
		auto grid2d = dynamic_cast<v4Structured2dGrid*>(gItem->grid()->grid());
		if (grid2d != nullptr) {
			// This is a structured 2d grid.
			vtkIdType i, j;
			grid2d->getPointIJIndex(pid, &i, &j);
			dialog.setIJ(i, j);
		} else {
			dialog.setIndex(pid);
		}
		if (QDialog::Accepted == dialog.exec()) {
			// new position was specified.
			auto pos = dialog.position();
			x[0] = pos.x();
			x[1] = pos.y();
			auto oldPoints = gItem->grid()->grid()->vtkData()->data()->GetPoints();
			auto newPoints = vtkSmartPointer<vtkPoints>::New();
			newPoints->DeepCopy(oldPoints);
			newPoints->SetPoint(pid, x);
			newPoints->Modified();
			pushRenderCommand(new UpdatePointsCommand(true, newPoints, gItem), this);
		}
	} else if (selVertices.size() > 1) {
		// more than one node is selected.
		PositionDeltaDialog dialog(preProcessorWindow());
		if (QDialog::Accepted == dialog.exec()) {
			// delta was specified.
			auto delta = dialog.delta();
			auto oldPoints = gItem->grid()->grid()->vtkData()->data()->GetPoints();
			auto newPoints = vtkSmartPointer<vtkPoints>::New();
			newPoints->DeepCopy(oldPoints);

			for (int i = 0; i < static_cast<int> (selVertices.size()); ++i) {
				vtkIdType pid = selVertices.at(i);
				double p[3];
				newPoints->GetPoint(pid, p);
				p[0] += delta.x();
				p[1] += delta.y();
				newPoints->SetPoint(pid, p);
			}
			newPoints->Modified();
			pushRenderCommand(new UpdatePointsCommand(true, newPoints, gItem), this);
		}
	}
}

void PreProcessorGridShapeDataItem::openCrossSectionWindow()
{
	auto gItem = gridDataItem();

	// use the first selected point
	unsigned int index = gItem->selectedNodesController()->selectedDataIds().at(0);
	vtkIdType i, j;

	v4InputGrid* g = gItem->grid();
	auto grid = dynamic_cast<v4Structured2dGrid*>(g->grid());
	grid->getPointIJIndex(index, &i, &j);

	gItem->openCrossSectionWindow(PreProcessorGridCrosssectionInternalWindow::Direction::I, i);
}

void PreProcessorGridShapeDataItem::openVerticalCrossSectionWindow()
{
	auto gItem = gridDataItem();

	// use the first selected point
	unsigned int index = gItem->selectedNodesController()->selectedDataIds().at(0);
	vtkIdType i, j;

	v4InputGrid* g = gItem->grid();
	auto grid = dynamic_cast<v4Structured2dGrid*>(g->grid());
	grid->getPointIJIndex(index, &i, &j);

	gItem->openCrossSectionWindow(PreProcessorGridCrosssectionInternalWindow::Direction::J, j);
}

void PreProcessorGridShapeDataItem::addCustomMenuItems(QMenu* /*menu*/)
{}

QAction* PreProcessorGridShapeDataItem::editAction() const
{
	return m_editAction;
}

QAction* PreProcessorGridShapeDataItem::openXsectionWindowAction() const
{
	return m_openXsectionWindowAction;
}

QAction* PreProcessorGridShapeDataItem::openVXsectionWindowAction() const
{
	return m_openVXsectionWindowAction;
}

void PreProcessorGridShapeDataItem::updateActionStatus()
{
	auto gItem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	bool verticesSelected = gItem->selectedNodesController()->selectedDataIds().size() > 0;

	m_editAction->setEnabled(verticesSelected);
	m_openXsectionWindowAction->setEnabled(verticesSelected);
	m_openVXsectionWindowAction->setEnabled(verticesSelected);
}

void PreProcessorGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	updateActorSetting();
}

void PreProcessorGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}


