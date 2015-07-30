#include "../preprocessordatamodel.h"
#include "../preprocessorwindow.h"
#include "gridshape/preprocessorgridshapedeltadialog.h"
#include "gridshape/preprocessorgridshapenewpositiondialog.h"
#include "preprocessorgridattributenodedataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem.h"

#include <guibase/coloreditwidget.h>
#include <guibase/graphicsmisc.h>
#include <guibase/gridshapeeditdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/project/projectdata.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPen>
#include <QPushButton>
#include <QSettings>
#include <QStandardItem>
#include <QString>
#include <QUndoCommand>
#include <QVBoxLayout>
#include <QXmlStreamWriter>

#include <vtkActorCollection.h>
#include <vtkCellArray.h>
#include <vtkCollectionIterator.h>
#include <vtkDataSet.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkPointLocator.h>
#include <vtkPointSet.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridOutlineFilter.h>
#include <vtkTriangle.h>
#include <vtkVertex.h>

PreProcessorGridShapeDataItem::PreProcessorGridShapeDataItem(PreProcessorDataItem* parent)
	: PreProcessorDataItem(tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_isDeletable = false;
	m_definingBoundingBox = false;
	m_draggingSelectedPoints = false;
	m_nearSelectedPoint = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	QSettings setting;
	QColor gridColor = setting.value("graphics/gridcolor", QColor(Qt::black)).value<QColor>();
	setColor(gridColor); // default is black.

	m_indexVisible = false;
	QColor indexColor = setting.value("graphics/gridindexcolor", QColor(Qt::black)).value<QColor>();
	setIndexColor(indexColor);

//	m_axesVisible = false;
//	QColor axesColor = setting.value("graphics/gridaxescolor", Qt::black).value<QColor>();
//	setAxesColor(axesColor);

	QList<QStandardItem*> rows = parent->standardItem()->takeRow(m_standardItem->row());

//	m_axesActor = vtkSmartPointer<vtkCubeAxesActor2D>::New();
//	iRIC::setupCubeActor2D(m_axesActor);
//	m_axesActor->SetZAxisVisibility(0);
//	m_axesActor->SetCamera(renderer()->GetActiveCamera());
//	m_axesActor->VisibilityOff();
//	renderer()->AddActor2D(m_axesActor);

	m_editAction = new QAction(tr("&Edit Coordinates..."), this);
	connect(m_editAction, SIGNAL(triggered()), this, SLOT(editShape()));

	m_openXsectionWindowAction = new QAction(tr("Open &Crosssection Window"), this);
	m_openXsectionWindowAction->setDisabled(true);
	connect(m_openXsectionWindowAction, SIGNAL(triggered()), this, SLOT(openCrossSectionWindow()));

	m_openVXsectionWindowAction = new QAction(tr("Open &Vertical Crosssection Window"), this);
	m_openVXsectionWindowAction->setDisabled(true);
	connect(m_openVXsectionWindowAction, SIGNAL(triggered()), this, SLOT(openVerticalCrossSectionWindow()));
}

PreProcessorGridShapeDataItem::~PreProcessorGridShapeDataItem()
{
//	renderer()->RemoveActor(m_axesActor);
}

void PreProcessorGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

QColor PreProcessorGridShapeDataItem::color()
{
	return QColor(
					 (int)(m_color[0] * 255),
					 (int)(m_color[1] * 255),
					 (int)(m_color[2] * 255));
}

void PreProcessorGridShapeDataItem::setColor(const QColor& color)
{
	m_color[0] = color.red()   / 255.;
	m_color[1] = color.green() / 255.;
	m_color[2] = color.blue()  / 255.;
}

QColor PreProcessorGridShapeDataItem::indexColor()
{
	return QColor(
					 (int)(m_indexColor[0] * 255),
					 (int)(m_indexColor[1] * 255),
					 (int)(m_indexColor[2] * 255));
}

void PreProcessorGridShapeDataItem::setIndexColor(const QColor& color)
{
	m_indexColor[0] = color.red()   / 255.;
	m_indexColor[1] = color.green() / 255.;
	m_indexColor[2] = color.blue()  / 255.;
}
/*
QColor PreProcessorGridShapeDataItem::axesColor()
{
	return QColor(
		(int) (m_axesColor[0] * 255),
		(int) (m_axesColor[1] * 255),
		(int) (m_axesColor[2] * 255));
}

void PreProcessorGridShapeDataItem::setAxesColor(const QColor& color)
{
	m_axesColor[0] = color.red()   / 255.;
	m_axesColor[1] = color.green() / 255.;
	m_axesColor[2] = color.blue()  / 255.;
}
*/
class GridPointMouseMoveCommand : public QUndoCommand
{
public:
	GridPointMouseMoveCommand(vtkPoints* newPoints, vtkPoints* oldPoints, bool first, PreProcessorGridDataItem* dItem)
		: QUndoCommand(PreProcessorGridShapeDataItem::tr("Move Grid Point(s)")) {
		m_newPoints = newPoints;
		m_oldPoints = oldPoints;
		m_dataItem = dItem;
		m_first = first;
	}
	void undo() {
		m_dataItem->grid()->vtkGrid()->SetPoints(m_oldPoints);
		m_dataItem->grid()->setModified();
		m_dataItem->updateRegionPolyData();
		m_dataItem->selectedVerticesPolyData()->SetPoints(m_oldPoints);
		m_dataItem->selectedVerticesPolyData()->Modified();
		m_dataItem->renderGraphicsView();
	}
	void redo() {
		m_dataItem->grid()->vtkGrid()->SetPoints(m_newPoints);
		m_dataItem->grid()->setModified();
		m_dataItem->updateRegionPolyData();
		m_dataItem->selectedVerticesPolyData()->SetPoints(m_newPoints);
		m_dataItem->selectedVerticesPolyData()->Modified();
		m_dataItem->renderGraphicsView();
	}
	int id() const {return iRIC::generateCommandId("GridPointMouseMove");}
	virtual bool mergeWith(const QUndoCommand* other) {
		const GridPointMouseMoveCommand* other2 = dynamic_cast<const GridPointMouseMoveCommand*>(other);
		if (other2 == nullptr) {return false;}
		if (other2->m_dataItem != m_dataItem) {return false;}
		if (other2->m_first) {return false;}
		m_newPoints = other2->m_newPoints;
		return true;
	}
private:
	vtkSmartPointer<vtkPoints> m_newPoints;
	vtkSmartPointer<vtkPoints> m_oldPoints;
	PreProcessorGridDataItem* m_dataItem;
	bool m_first;
};

void PreProcessorGridShapeDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	PreProcessorGraphicsViewInterface* v2 = dynamic_cast<PreProcessorGraphicsViewInterface*>(v);
	if (m_draggingSelectedPoints) {
		PreProcessorGridDataItem* p = dynamic_cast<PreProcessorGridDataItem*>(parent());
		vtkSmartPointer<vtkPoints> newpoints = vtkSmartPointer<vtkPoints>::New();
		newpoints->DeepCopy(m_pointsBeforeDragging);
		double x = event->x();
		double y = event->y();
		v2->viewportToWorld(x, y);
		QVector2D dragEnd(x, y);
		QVector2D delta = dragEnd - m_dragStartPoint;
		const QVector<vtkIdType>& selVertices = p->selectedVertices();
		for (int i = 0; i < selVertices.count(); ++i) {
			double x[3];
			vtkIdType vid = selVertices.at(i);
			m_pointsBeforeDragging->GetPoint(vid, x);
			x[0] += delta.x();
			x[1] += delta.y();
			newpoints->SetPoint(vid, x);
		}
		newpoints->Modified();
		iRICUndoStack::instance().push(new GridPointMouseMoveCommand(newpoints, m_pointsBeforeDragging, false, p));
	} else if (m_definingBoundingBox) {
		// drawing bounding box using mouse dragging.
		dynamic_cast<PreProcessorGridDataItem*>(parent())->nodeSelectingMouseMoveEvent(event, v);
	} else {
		// free mouse cursor move.
		// ingestigate whether there exists a selected point
		// near enough to the mouse pointer.
		PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());

		if (tmpparent->selectedVertices().count() == 0) {return;}
		if (tmpparent->grid()->isMasked()) {
			v->setCursor(Qt::ForbiddenCursor);
			return;
		} else {
			v->setCursor(Qt::ArrowCursor);
		}

		double x = event->x();
		double y = event->y();
		v2->viewportToWorld(x, y);
		double p[3];
		p[0] = x;
		p[1] = y;
		p[2] = 0;
		double radius = v2->stdRadius(4);
		double dist2;

		vtkSmartPointer<vtkPointLocator> locator = vtkSmartPointer<vtkPointLocator>::New();
		locator->SetDataSet(tmpparent->grid()->vtkGrid());
		vtkIdType closestId = locator->FindClosestPointWithinRadius(radius, p, dist2);
		if (closestId >= 0) {
			const QVector<vtkIdType>& selectedVerts = tmpparent->selectedVertices();
			m_nearSelectedPoint = (selectedVerts.contains(closestId));
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
			PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
			// start dragging of the selected points.
			m_draggingSelectedPoints = true;
			m_pointsBeforeDragging = vtkSmartPointer<vtkPoints>::New();
			m_pointsBeforeDragging->DeepCopy(tmpparent->grid()->vtkGrid()->GetPoints());
			v->setCursor(Qt::ClosedHandCursor);
			double x = event->x();
			double y = event->y();
			v2->viewportToWorld(x, y);
			m_dragStartPoint.setX(x);
			m_dragStartPoint.setY(y);
			iRICUndoStack::instance().push(new GridPointMouseMoveCommand(m_pointsBeforeDragging, m_pointsBeforeDragging, true, tmpparent));
		} else {
			// start drawing the mouse bounding box.
			m_definingBoundingBox = true;
			dynamic_cast<PreProcessorGridDataItem*>(parent())->nodeSelectingMousePressEvent(event, v);
		}
	}
	m_pressPoint = QPoint(event->x(), event->y());
}

void PreProcessorGridShapeDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	static QMenu* menu = nullptr;
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
	if (event->button() == Qt::LeftButton) {
		if (m_definingBoundingBox) {
			tmpparent->nodeSelectingMouseReleaseEvent(event, v);
		}
		m_definingBoundingBox = false;
		m_draggingSelectedPoints = false;
	} else if (event->button() == Qt::RightButton) {
		QPoint releasePoint(event->x(), event->y());
		if (iRIC::isNear(m_pressPoint, releasePoint)) {
			delete menu;
			menu = new QMenu(projectData()->mainWindow());
			menu->addAction(m_editAction);

			Structured2DGrid* grid2d = dynamic_cast<Structured2DGrid*>(tmpparent->grid());
			if (grid2d != nullptr && grid2d->gridRelatedCondition("Elevation") != nullptr) {
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
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
	tmpparent->nodeSelectingKeyPressEvent(event, v);
}

void PreProcessorGridShapeDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
	tmpparent->nodeSelectingKeyReleaseEvent(event, v);
}

void PreProcessorGridShapeDataItem::editShape()
{
	iRICMainWindowInterface* mw = dataModel()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
	const QVector<vtkIdType>& selVertices = tmpparent->selectedVertices();

	if (selVertices.count() == 1) {
		// only one node is selected.
		PreProcessorGridShapeNewPositionDialog dialog(preProcessorWindow());
		PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
		vtkIdType pid = selVertices.at(0);
		double x[3];
		tmpparent->grid()->vtkGrid()->GetPoint(pid, x);
		dialog.setPosition(x[0], x[1]);
		Structured2DGrid* grid2d = dynamic_cast<Structured2DGrid*>(tmpparent->grid());
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
			QVector2D pos = dialog.position();
			x[0] = pos.x();
			x[1] = pos.y();
			vtkSmartPointer<vtkPoints> oldPoints = tmpparent->grid()->vtkGrid()->GetPoints();
			vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
			newPoints->DeepCopy(oldPoints);
			newPoints->SetPoint(pid, x);
			newPoints->Modified();
			iRICUndoStack::instance().push(new GridPointMouseMoveCommand(newPoints, oldPoints, true, tmpparent));
		}
	} else if (selVertices.count() > 1) {
		// more than one node is selected.
		PreProcessorGridShapeDeltaDialog dialog(preProcessorWindow());
		if (QDialog::Accepted == dialog.exec()) {
			PreProcessorGridDataItem* tmpparent = dynamic_cast<PreProcessorGridDataItem*>(parent());
			// delta was specified.
			QVector2D delta = dialog.delta();
			vtkSmartPointer<vtkPoints> oldPoints = tmpparent->grid()->vtkGrid()->GetPoints();
			vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
			newPoints->DeepCopy(oldPoints);

			for (int i = 0; i < selVertices.count(); ++i) {
				vtkIdType pid = selVertices.at(i);
				double p[3];
				newPoints->GetPoint(pid, p);
				p[0] += delta.x();
				p[1] += delta.y();
				newPoints->SetPoint(pid, p);
			}
			newPoints->Modified();
			iRICUndoStack::instance().push(new GridPointMouseMoveCommand(newPoints, oldPoints, true, tmpparent));
		}
	}
}

void PreProcessorGridShapeDataItem::openCrossSectionWindow()
{
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	PreProcessorGridAttributeNodeDataItem* nItem = gItem->nodeGroupDataItem()->nodeDataItem("Elevation");
	if (nItem != nullptr) {
		nItem->openCrossSectionWindow();
	}
}

void PreProcessorGridShapeDataItem::openVerticalCrossSectionWindow()
{
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	PreProcessorGridAttributeNodeDataItem* nItem = gItem->nodeGroupDataItem()->nodeDataItem("Elevation");
	if (nItem != nullptr) {
		nItem->openVerticalCrossSectionWindow();
	}
}

void PreProcessorGridShapeDataItem::addCustomMenuItems(QMenu* /*menu*/)
{
}

void PreProcessorGridShapeDataItem::updateActionStatus()
{
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	m_editAction->setEnabled(gItem->selectedVertices().count() > 0);
	m_openXsectionWindowAction->setEnabled(gItem->selectedVertices().count() > 0);
	m_openVXsectionWindowAction->setEnabled(gItem->selectedVertices().count() > 0);
}
/*
void PreProcessorGridShapeDataItem::updateAxesRegion()
{
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g == nullptr){return;}
	double bounds[6];
	g->vtkGrid()->GetBounds(bounds);
	if (! m_xAxisSetting.isAuto){
		bounds[0] = m_xAxisSetting.min;
		bounds[1] = m_xAxisSetting.max;
	}
	if (! m_yAxisSetting.isAuto){
		bounds[2] = m_yAxisSetting.min;
		bounds[3] = m_yAxisSetting.max;
	}
	m_axesActor->SetBounds(bounds);
}
*/
void PreProcessorGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();

	setColor(iRIC::QColorFromString(elem.attribute("color", "#000000")));
	m_indexVisible = static_cast<bool>(elem.attribute("indexvisible", "0").toInt());
	setIndexColor(iRIC::QColorFromString(elem.attribute("indexcolor", "#000000")));
//	m_axesVisible = static_cast<bool>(elem.attribute("axesvisible", "0").toInt());
//	setAxesColor(iRIC::QColorFromString(elem.attribute("axescolor", "#000000")));
}

void PreProcessorGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString boolStr;
	writer.writeAttribute("color", iRIC::QColorToString(color()));

	boolStr.setNum(m_indexVisible);
	writer.writeAttribute("indexvisible", boolStr);
	writer.writeAttribute("indexcolor", iRIC::QColorToString(indexColor()));

//	boolStr.setNum(m_axesVisible);
//	writer.writeAttribute("axesvisible", boolStr);
//	writer.writeAttribute("axescolor", iRIC::QColorToString(axesColor()));
}


