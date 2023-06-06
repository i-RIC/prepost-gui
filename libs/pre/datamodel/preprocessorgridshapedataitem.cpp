#include "../preprocessordatamodel.h"
#include "../preprocessorwindow.h"
#include "../subwindow/gridcrosssectionwindow2/preprocessorgridcrosssectionwindow2.h"
#include "preprocessorgridattributenodedataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem.h"
#include "private/preprocessorgridshapedataitem_updatepointscommand.h"
#include "private/preprocessorgridshapedataitem_positiondeltadialog.h"
#include "private/preprocessorgridshapedataitem_positioneditdialog.h"

#include <guibase/widget/coloreditwidget.h>
#include <guibase/graphicsmisc.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/project/projectdata.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QGraphicsItemGroup>
#include <QMenu>
#include <QSettings>
#include <QStandardItem>
#include <QString>
#include <QXmlStreamWriter>

#include <vtkActorCollection.h>
#include <vtkCellArray.h>
#include <vtkCollectionIterator.h>
#include <vtkPointSet.h>
#include <vtkRenderer.h>

#include <algorithm>

PreProcessorGridShapeDataItem::PreProcessorGridShapeDataItem(PreProcessorDataItem* parent) :
	PreProcessorDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.svg"), parent},
	m_definingBoundingBox {false},
	m_draggingSelectedPoints {false},
	m_nearSelectedPoint {false}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	parent->standardItem()->takeRow(m_standardItem->row());

	m_setting.shape = GridShapeEditDialog::Wireframe;

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
	PreProcessorGraphicsViewInterface* v2 = dynamic_cast<PreProcessorGraphicsViewInterface*>(v);
	if (m_draggingSelectedPoints) {
		auto p = dynamic_cast<PreProcessorGridDataItem*>(parent());
		auto newpoints = vtkSmartPointer<vtkPoints>::New();
		newpoints->DeepCopy(m_pointsBeforeDragging);
		auto dragEnd = v2->viewportToWorld(event->pos());
		auto delta = dragEnd - m_dragStartPoint;
		const auto& selVertices = p->selectedVertices();
		for (int i = 0; i < static_cast<int> (selVertices.size()); ++i) {
			double x[3];
			vtkIdType vid = selVertices.at(i);
			m_pointsBeforeDragging->GetPoint(vid, x);
			x[0] += delta.x();
			x[1] += delta.y();
			newpoints->SetPoint(vid, x);
		}
		newpoints->Modified();
		pushRenderCommand(new UpdatePointsCommand(false, newpoints, p), this);
	} else if (m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		dynamic_cast<PreProcessorGridDataItem*>(parent())->nodeSelectingMouseMoveEvent(event, v);
	} else {
		// free mouse cursor move.
		// ingestigate whether there exists a selected point
		// near enough to the mouse pointer.
		auto tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());

		if (tmpparent->selectedVertices().size() == 0) {return;}
		if (tmpparent->grid()->isMasked()) {
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

		auto locator = tmpparent->grid()->pointLocator();
		vtkIdType closestId = locator->FindClosestPointWithinRadius(radius, p, dist2);
		if (closestId >= 0) {
			const auto& selectedVerts = tmpparent->selectedVertices();
			auto it = std::lower_bound(selectedVerts.begin(), selectedVerts.end(), closestId);
			m_nearSelectedPoint = (it != selectedVerts.end() && *it == closestId);
		} else {
			m_nearSelectedPoint = false;
		}
		if (m_nearSelectedPoint) {
			v->setCursor(Qt::OpenHandCursor);
		} else {
			v->setCursor(tmpparent->normalCursor());
		}
	}
}

void PreProcessorGridShapeDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	PreProcessorGraphicsViewInterface* v2 = dynamic_cast<PreProcessorGraphicsViewInterface*>(v);
	if (event->button() == Qt::LeftButton) {
		if (m_nearSelectedPoint) {
			iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
			if (mw->isSolverRunning()) {
				mw->warnSolverRunning();
				return;
			}
			auto tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
			// start dragging of the selected points.
			m_draggingSelectedPoints = true;
			m_pointsBeforeDragging = vtkSmartPointer<vtkPoints>::New();
			m_pointsBeforeDragging->DeepCopy(tmpparent->grid()->vtkGrid()->GetPoints());
			m_dragStartPoint = v2->viewportToWorld(event->pos());
			v->setCursor(Qt::ClosedHandCursor);
		} else {
			// start drawing the mouse bounding box.
			m_definingBoundingBox = true;
			dynamic_cast<PreProcessorGridDataItem*>(parent())->nodeSelectingMousePressEvent(event, v);
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
			auto newPoints = gItem->grid()->vtkGrid()->GetPoints();
			newPoints->Register(nullptr);
			pushRenderCommand(new UpdatePointsCommand(true, newPoints, gItem), this);
		} else if (m_definingBoundingBox) {
			auto v2 = dynamic_cast<VTK2DGraphicsView*> (v);
			gItem->nodeSelectingMouseReleaseEvent(event, v2);
		}
		m_definingBoundingBox = false;
		m_draggingSelectedPoints = false;
	} else if (event->button() == Qt::RightButton) {
		QPoint releasePoint(event->x(), event->y());
		if (iRIC::isNear(m_pressPoint, releasePoint)) {
			delete menu;
			menu = new QMenu(projectData()->mainWindow());
			menu->addAction(m_editAction);

			auto grid2d = dynamic_cast<Structured2DGrid*>(gItem->grid());
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
	gridDataItem()->nodeSelectingKeyPressEvent(event, v);
}

void PreProcessorGridShapeDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
	tmpparent->nodeSelectingKeyReleaseEvent(event, v);
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

	auto tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
	const auto& selVertices = tmpparent->selectedVertices();

	if (selVertices.size() == 1) {
		// only one node is selected.
		PositionEditDialog dialog(preProcessorWindow());
		auto tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
		vtkIdType pid = selVertices.at(0);
		double x[3];
		tmpparent->grid()->vtkGrid()->GetPoint(pid, x);
		dialog.setPosition(x[0], x[1]);
		auto  grid2d = dynamic_cast<Structured2DGrid*>(tmpparent->grid());
		if (grid2d != nullptr) {
			// This is a structured 2d grid.
			unsigned int i, j;
			grid2d->getIJIndex(pid, &i, &j);
			dialog.setIJ(i, j);
		} else {
			dialog.setIndex(pid);
		}
		if (QDialog::Accepted == dialog.exec()) {
			// new position was specified.
			auto pos = dialog.position();
			x[0] = pos.x();
			x[1] = pos.y();
			auto oldPoints = tmpparent->grid()->vtkGrid()->GetPoints();
			auto newPoints = vtkSmartPointer<vtkPoints>::New();
			newPoints->DeepCopy(oldPoints);
			newPoints->SetPoint(pid, x);
			newPoints->Modified();
			pushRenderCommand(new UpdatePointsCommand(true, newPoints, tmpparent), this);
		}
	} else if (selVertices.size() > 1) {
		// more than one node is selected.
		PositionDeltaDialog dialog(preProcessorWindow());
		if (QDialog::Accepted == dialog.exec()) {
			auto tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
			// delta was specified.
			auto delta = dialog.delta();
			auto oldPoints = tmpparent->grid()->vtkGrid()->GetPoints();
			auto newPoints = vtkSmartPointer<vtkPoints>::New();
			newPoints->DeepCopy(oldPoints);

			for (int i = 0; i < selVertices.size(); ++i) {
				vtkIdType pid = selVertices.at(i);
				double p[3];
				newPoints->GetPoint(pid, p);
				p[0] += delta.x();
				p[1] += delta.y();
				newPoints->SetPoint(pid, p);
			}
			newPoints->Modified();
			pushRenderCommand(new UpdatePointsCommand(true, newPoints, tmpparent), this);
		}
	}
}

void PreProcessorGridShapeDataItem::openCrossSectionWindow()
{
	auto gItem = gridDataItem();

	// use the first selected point
	unsigned int index = gItem->selectedVertices().at(0);
	unsigned int i, j;

	Grid* g = gItem->grid();
	auto grid = dynamic_cast<Structured2DGrid*>(g);
	grid->getIJIndex(index, &i, &j);

	gItem->openCrossSectionWindow(PreProcessorGridCrosssectionWindow2::Direction::I, i);
}

void PreProcessorGridShapeDataItem::openVerticalCrossSectionWindow()
{
	auto gItem = gridDataItem();

	// use the first selected point
	unsigned int index = gItem->selectedVertices().at(0);
	unsigned int i, j;

	Grid* g = gItem->grid();
	auto grid = dynamic_cast<Structured2DGrid*>(g);
	grid->getIJIndex(index, &i, &j);

	gItem->openCrossSectionWindow(PreProcessorGridCrosssectionWindow2::Direction::J, j);
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
	bool verticesSelected = gItem->selectedVertices().size() > 0;

	m_editAction->setEnabled(verticesSelected);
	m_openXsectionWindowAction->setEnabled(verticesSelected);
	m_openVXsectionWindowAction->setEnabled(verticesSelected);
}

void PreProcessorGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
}

void PreProcessorGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}


