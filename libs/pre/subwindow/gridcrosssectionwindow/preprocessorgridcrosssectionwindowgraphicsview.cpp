#include "../../datamodel/preprocessorgridattributenodedataitem.h"
#include "../../datamodel/preprocessorgriddataitem.h"
#include "preprocessorgridcrosssectionwindow.h"
#include "preprocessorgridcrosssectionwindowgraphicsview.h"
#include "preprocessorgridcrosssectionwindowprojectdataitem.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributeeditdialog.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/project/projectdataitem.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>

#include <QAction>
#include <QItemSelection>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndexList>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <QSet>
#include <QTableView>
#include <QWheelEvent>

#include <cmath>

class Structured2DGridCrosssectionEditCommand : public QUndoCommand
{
public:
	Structured2DGridCrosssectionEditCommand(bool apply, const QVector<vtkIdType>& indices, const QVector<double>& before, const QVector<double>& after, PreProcessorGridCrosssectionWindow* w, GridAttributeRealNodeContainer* cont, PreProcessorGridAttributeNodeDataItem* dItem) {
		m_apply = apply;
		m_indices = indices;
		m_before = before;
		m_after = after;
		m_window = w;
		m_container = cont;
		m_dataItem = dItem;
		m_first = true;
	}
	void redo() {
		for (int i = 0; i < m_indices.count(); ++i) {
			m_container->setValue(m_indices.at(i), m_after.at(i));
		}
		m_container->setModified();
		m_container->grid()->setModified();
		m_container->setCustomModified(true);
		if (m_apply) {
			m_window->updateView();
		} else {
			m_dataItem->renderGraphicsView();
			m_dataItem->updateCrossectionWindows();
		}
		m_first = false;
	}
	void undo() {
		for (int i = 0; i < m_indices.count(); ++i) {
			m_container->setValue(m_indices.at(i), m_before.at(i));
		}
		m_container->setModified();
		m_container->grid()->setModified();
		if (! m_apply) {
			m_dataItem->renderGraphicsView();
			m_dataItem->updateCrossectionWindows();
		}
	}
	int id() const {
		return iRIC::generateCommandId("Structured2DGridCrosssectionEdit");
	}
	bool mergeWith(const QUndoCommand* other) {
		const Structured2DGridCrosssectionEditCommand* comm = dynamic_cast<const Structured2DGridCrosssectionEditCommand*>(other);
		if (m_window != comm->m_window) {return false;}
		if (m_container != comm->m_container) {return false;}
		if (m_dataItem != comm->m_dataItem) {return false;}
		if (m_indices != comm->m_indices) {return false;}
		if (m_apply == false) {return false;}

		m_after = comm->m_after;
		m_apply = comm->m_apply;
		return true;
	}

private:
	bool m_apply;
	bool m_first;
	QVector<vtkIdType> m_indices;
	QVector<double> m_before;
	QVector<double> m_after;
	PreProcessorGridCrosssectionWindow* m_window;
	GridAttributeRealNodeContainer* m_container;
	PreProcessorGridAttributeNodeDataItem* m_dataItem;
};

PreProcessorGridCrosssectionWindowGraphicsView::PreProcessorGridCrosssectionWindowGraphicsView(QWidget* w)
	: QAbstractItemView(w)
{
	fLeftMargin = 0.1f;
	fRightMargin = 0.1f;
	fTopMargin = 0.1f;
	fBottomMargin = 0.1f;
	m_mouseEventMode = meNormal;
	m_rubberBand = 0;
	m_rightClickingMenu = nullptr;

	// Set cursors for mouse view change events.
	m_zoomPixmap = QPixmap(":/libs/guibase/images/cursorZoom.png");
	m_movePixmap = QPixmap(":/libs/guibase/images/cursorMove.png");

	m_zoomCursor = QCursor(m_zoomPixmap);
	m_moveCursor = QCursor(m_movePixmap);
	setMouseTracking(true);
	setupActions();
}

void PreProcessorGridCrosssectionWindowGraphicsView::setupActions()
{
	m_editAction = new QAction(tr("&Edit..."), this);
	connect(m_editAction, SIGNAL(triggered()), this, SLOT(moveSelectedRows()));
	m_editAction->setEnabled(false);
}

void PreProcessorGridCrosssectionWindowGraphicsView::setupMenu()
{
	if (m_rightClickingMenu == nullptr) {
		m_rightClickingMenu = new QMenu(this);
		m_rightClickingMenu->addAction(m_editAction);
	}
}

void PreProcessorGridCrosssectionWindowGraphicsView::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/)
{
	viewport()->update();
}

void PreProcessorGridCrosssectionWindowGraphicsView::paintEvent(QPaintEvent* /*e*/)
{
	QPainter painter(viewport());
	QRect vp = painter.viewport();
	QMatrix matrix = getMatrix(vp);
	m_matrix = matrix;
	painter.setRenderHint(QPainter::Antialiasing);
	// Draw scales.
	drawScales(painter, matrix);

	// draw lines.
	// black lines
	QPen pen = QPen(Qt::black, 1);
	painter.setPen(pen);
	drawLine(m_parentWindow->m_blackLineIndex, painter);
	// red lines
	pen = QPen(Qt::red, 1);
	painter.setPen(pen);
	if (m_parentWindow->m_redLineIndex != - 1) {
		drawLine(m_parentWindow->m_redLineIndex, painter);
	}
	// blue lines
	pen = QPen(Qt::blue, 1);
	painter.setPen(pen);
	if (m_parentWindow->m_blueLineIndex != - 1) {
		drawLine(m_parentWindow->m_blueLineIndex, painter);
	}
	// draw circles.
	drawCircle(painter);
	// draw selected circles.
	drawSelectionCircle(painter);
}

void PreProcessorGridCrosssectionWindowGraphicsView::drawLine(int index, QPainter& painter)
{
	Structured2DGrid* grid = m_parentWindow->grid();
	GridAttributeContainer* cond = grid->gridAttribute(m_parentWindow->condition());
	GridAttributeRealNodeContainer* cond2 = dynamic_cast<GridAttributeRealNodeContainer*>(cond);
	double distance = 0;
	if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirI) {
		double v = cond2->value(grid->vertexIndex(0, index));
		QPointF p0 = m_matrix.map(QPointF(distance, v));
		for (unsigned int i = 1; i < grid->dimensionI(); ++i) {
			distance += getDistance(index, i - 1, i);
			v = cond2->value(grid->vertexIndex(i, index));
			QPointF p1 = m_matrix.map(QPointF(distance, v));
			painter.drawLine(p0, p1);
			p0 = p1;
		}
	} else if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirJ) {
		double offset = getDistance(index, 0, grid->dimensionJ() - 1) * 0.5;
		double v = cond2->value(grid->vertexIndex(index, 0));
		QPointF p0 = m_matrix.map(QPointF(offset - distance, v));
		for (unsigned int j = 1; j < grid->dimensionJ(); ++j) {
			distance += getDistance(index, j - 1, j);
			v = cond2->value(grid->vertexIndex(index, j));
			QPointF p1 = m_matrix.map(QPointF(offset - distance, v));
			painter.drawLine(p0, p1);
			p0 = p1;
		}
	}
}

void PreProcessorGridCrosssectionWindowGraphicsView::drawCircle(QPainter& painter)
{
	Structured2DGrid* grid = m_parentWindow->grid();
	GridAttributeContainer* cond = grid->gridAttribute(m_parentWindow->condition());
	GridAttributeRealNodeContainer* cond2 = dynamic_cast<GridAttributeRealNodeContainer*>(cond);
	double distance = 0;
	QPen pen(Qt::black, 1, Qt::SolidLine);
	QBrush activeBrush(Qt::red, Qt::SolidPattern);
	painter.setPen(pen);
	painter.setBrush(activeBrush);

	if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirI) {
		for (unsigned int i = 0; i < grid->dimensionI(); ++i) {
			if (i == 0) {
				distance = 0;
			} else {
				distance += getDistance(m_parentWindow->targetIndex(), i - 1, i);
			}
			double v = cond2->value(grid->vertexIndex(i, m_parentWindow->targetIndex()));
			QPointF point = m_matrix.map(QPointF(distance, v));
			QRectF r(point.x() - ellipseR, point.y() - ellipseR, ellipseR * 2, ellipseR * 2);
			painter.drawEllipse(r);
		}
	} else if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirJ) {
		double offset = getDistance(m_parentWindow->targetIndex(), 0, grid->dimensionJ() - 1) * 0.5;
		for (unsigned int j = 0; j < grid->dimensionJ(); ++j) {
			if (j == 0) {
				distance = 0;
			} else {
				distance += getDistance(m_parentWindow->targetIndex(), j - 1, j);
			}
			double v = cond2->value(grid->vertexIndex(m_parentWindow->targetIndex(), j));
			QPointF point = m_matrix.map(QPointF(offset - distance, v));
			QRectF r(point.x() - ellipseR, point.y() - ellipseR, ellipseR * 2, ellipseR * 2);
			painter.drawEllipse(r);
		}
	}
}

void PreProcessorGridCrosssectionWindowGraphicsView::drawSelectionCircle(QPainter& painter)
{
	Structured2DGrid* grid = m_parentWindow->grid();
	GridAttributeContainer* cond = grid->gridAttribute(m_parentWindow->condition());
	GridAttributeRealNodeContainer* cond2 = dynamic_cast<GridAttributeRealNodeContainer*>(cond);
	QPen pen(Qt::black, 1, Qt::SolidLine);
	QBrush activeBrush(Qt::red, Qt::SolidPattern);
	painter.setPen(pen);
	painter.setBrush(activeBrush);

	QModelIndexList list = selectionModel()->selectedIndexes();
	QSet<int> drawnRows;

	if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirI) {
		for (auto it = list.begin(); it != list.end(); ++it) {
			QModelIndex index = *it;
			if (drawnRows.contains(index.row())) {continue;}
			double distance = getDistance(m_parentWindow->targetIndex(), 0, index.row());
			double v = cond2->value(grid->vertexIndex(index.row(), m_parentWindow->targetIndex()));
			QPointF point = m_matrix.map(QPointF(distance, v));
			QRectF r(point.x() - selectedEllipseR, point.y() - selectedEllipseR, selectedEllipseR * 2, selectedEllipseR * 2);
			painter.drawEllipse(r);
			drawnRows.insert(index.row());
		}
	} else if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirJ) {
		double offset = getDistance(m_parentWindow->targetIndex(), 0, grid->dimensionJ() - 1) * 0.5;
		for (auto it = list.begin(); it != list.end(); ++it) {
			QModelIndex index = *it;
			if (drawnRows.contains(index.row())) {continue;}
			double distance = getDistance(m_parentWindow->targetIndex(), 0, index.row());
			double v = cond2->value(grid->vertexIndex(m_parentWindow->targetIndex(), index.row()));
			QPointF point = m_matrix.map(QPointF(offset - distance, v));
			QRectF r(point.x() - selectedEllipseR, point.y() - selectedEllipseR, selectedEllipseR * 2, selectedEllipseR * 2);
			painter.drawEllipse(r);
			drawnRows.insert(index.row());
		}
	}
}

void PreProcessorGridCrosssectionWindowGraphicsView::drawScales(QPainter& painter, const QMatrix& matrix)
{
	QWidget* w = viewport();
	QMatrix invMatrix = matrix.inverted();
	QPointF mins, maxs;
	mins = invMatrix.map(QPointF(0, w->height()));
	maxs = invMatrix.map(QPointF(w->width(), 0));

	// set pen.
	QPen oldPen = painter.pen();
	QPen scalePen(QColor(160, 160, 50), 1, Qt::SolidLine, Qt::RoundCap);
	painter.setPen(scalePen);

	double xoffset = 5;
	double yoffset = 5;
	double fontoffset = 4;
	double mainruler = 5;
	double subruler = 3;

	double xdwidth = (maxs.x() - mins.x()) / 3;
	int i = 0;
	while (xdwidth > 10) {
		xdwidth /= 10.;
		++i;
	}
	while (xdwidth < 1) {
		xdwidth *= 10.;
		--i;
	}
	// now 1 < xdwidth < 10.
	double dx;
	double pow10 = 10;
	if (i < 0) {
		pow10 = 0.1;
		i = - i;
	}
	if (xdwidth > 5) {
		xdwidth = 5 * std::pow(pow10, i);
		dx = 0.2;
	} else if (xdwidth > 2) {
		xdwidth = 2 * std::pow(pow10, i);
		dx = 0.5;
	} else {
		xdwidth = std::pow(pow10, i);
		dx = 1.0;
	}
	double rulemin = std::floor(mins.x() / xdwidth) * xdwidth;

	// draw X main scales.
	QPointF from, to;
	from = QPointF(0, yoffset);
	to = QPointF(w->width(), yoffset);
	painter.drawLine(from, to);

	double x = rulemin;
	while (x < maxs.x()) {
		from = matrix.map(QPointF(x, maxs.y()));
		from.setY(yoffset);
		to = matrix.map(QPointF(x, maxs.y()));
		to.setY(yoffset + mainruler);
		painter.drawLine(from, to);
		QPointF fontPos = to;
		fontPos.setY(yoffset + mainruler + fontoffset);
		QRectF fontRect(QPointF(fontPos.x() - fontRectWidth / 2, fontPos.y()), QPointF(fontPos.x() + fontRectWidth / 2, fontPos.y() + fontRectHeight));
		QString str = QString("%1").arg(x);
		painter.drawText(fontRect, Qt::AlignHCenter | Qt::AlignTop, str);
		x += xdwidth;
	}
	// draw X sub scales.
	x = rulemin;
	while (x < maxs.x()) {
		x += xdwidth * dx;
		from = matrix.map(QPointF(x, maxs.y()));
		from.setY(from.y() + yoffset);
		to = matrix.map(QPointF(x, maxs.y()));
		to.setY(to.y() + yoffset + subruler);
		painter.drawLine(from, to);
	}

	// next, for y.
	double ydwidth = std::abs((maxs.y() - mins.y()) / 3);
	i = 0;
	while (ydwidth > 10) {
		ydwidth /= 10.;
		++i;
	}
	while (ydwidth < 1) {
		ydwidth *= 10.;
		--i;
	}
	// now 1 < ydwidth < 10.
	double dy;
	pow10 = 10;
	if (i < 0) {
		pow10 = 0.1;
		i = - i;
	}
	if (ydwidth > 5) {
		ydwidth = 5 * std::pow(pow10, i);
		dy = 0.2;
	} else if (ydwidth > 2) {
		ydwidth = 2 * std::pow(pow10, i);
		dy = 0.5;
	} else {
		ydwidth = std::pow(pow10, i);
		dy = 1.0;
	}
	rulemin = std::floor(mins.y() / ydwidth) * ydwidth;

	// draw Y main scales.
	from = QPointF(xoffset, 0);
	to = QPointF(xoffset, w->height());
	painter.drawLine(from, to);

	double y = rulemin;
	while (y < maxs.y()) {
		from = matrix.map(QPointF(mins.x(), y));
		from.setX(xoffset);
		to.setX(xoffset + mainruler);
		to.setY(from.y());
		painter.drawLine(from, to);
		QPointF fontPos = to;
		fontPos.setX(xoffset + mainruler + fontoffset);
		QRectF fontRect(QPointF(fontPos.x(), fontPos.y() - fontRectHeight / 2), QPointF(fontPos.x() + fontRectWidth, fontPos.y() + fontRectHeight / 2));
		QString str = QString("%1").arg(y);
		painter.drawText(fontRect, Qt::AlignLeft | Qt::AlignVCenter, str);
		y += ydwidth;
	}
	// draw Y sub scales.
	y = rulemin;
	while (y < maxs.y()) {
		y += ydwidth * dy;
		from = matrix.map(QPointF(mins.x(), y));
		from.setX(xoffset);
		to.setX(xoffset + subruler);
		to.setY(from.y());
		painter.drawLine(from, to);
	}

	if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirJ) {
		// line at x = 0;
		from = matrix.map(QPointF(0, 0));
		from.setY(0);
		to.setX(from.x());
		to.setY(w->height());
		scalePen.setStyle(Qt::DotLine);
		painter.drawLine(from, to);

		// left bank
		QRectF fontRect;
		fontRect = QRectF(from.x() - bankHOffset - fontRectWidth, yoffset + bankVOffset, fontRectWidth, fontRectHeight);
		painter.drawText(fontRect, Qt::AlignRight | Qt::AlignVCenter, tr("Left Bank Side"));

		// right bank side
		fontRect = QRectF(from.x() + bankHOffset, yoffset + bankVOffset, fontRectWidth, fontRectHeight);
		painter.drawText(fontRect, Qt::AlignLeft | Qt::AlignVCenter, tr("Right Bank Side"));
		painter.setPen(oldPen);
	}
}
QRectF PreProcessorGridCrosssectionWindowGraphicsView::getRegion()
{
	QRectF ret(0., 0., 0., 0.);
	double distance;
	double valmin, valmax;
	Structured2DGrid* grid = m_parentWindow->grid();
	GridAttributeContainer* cond = grid->gridAttribute(m_parentWindow->condition());
	GridAttributeRealNodeContainer* cond2 = dynamic_cast<GridAttributeRealNodeContainer*>(cond);
	if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirI) {
		distance = getDistance(m_parentWindow->targetIndex(), 0, grid->dimensionI() - 1);
		double v = cond2->value(grid->vertexIndex(0, m_parentWindow->targetIndex()));
		valmin = v;
		valmax = v;
		for (unsigned int i = 1; i < grid->dimensionI(); ++i) {
			v = cond2->value(grid->vertexIndex(i, m_parentWindow->targetIndex()));
			if (v < valmin) {valmin = v;}
			if (v > valmax) {valmax = v;}
		}
		if (valmin == valmax) {
			double center = valmin;
			valmin = center - 0.5;
			valmax = center + 0.5;
		}
		ret.setLeft(- 0.1 * distance);
		ret.setRight(1.1 * distance);
		ret.setTop(valmin);
		ret.setBottom(valmax);
		return ret;
	} else if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirJ) {
		distance = getDistance(m_parentWindow->targetIndex(), 0, grid->dimensionJ() - 1);
		double v = cond2->value(grid->vertexIndex(m_parentWindow->targetIndex(), 0));
		valmin = v;
		valmax = v;
		for (unsigned int j = 1; j < grid->dimensionJ(); ++j) {
			v = cond2->value(grid->vertexIndex(m_parentWindow->targetIndex(), j));
			if (v < valmin) {valmin = v;}
			if (v > valmax) {valmax = v;}
		}
		if (valmin == valmax) {
			double center = valmin;
			valmin = center - 0.5;
			valmax = center + 0.5;
		}
		ret.setLeft(- 0.65 * distance);
		ret.setRight(0.65 * distance);
		ret.setTop(valmin);
		ret.setBottom(valmax);
		return ret;
	}
	return ret;
}

QMatrix PreProcessorGridCrosssectionWindowGraphicsView::getMatrix(QRect& viewport)
{
	QRectF region = m_drawnRegion;
	QMatrix translate1, scale, translate2;
	double xlength = region.right() - region.left();
	double ylength = region.bottom() - region.top();
	if (xlength == 0) {xlength = 1;}
	if (ylength == 0) {ylength = 1;}

	translate1 = QMatrix(1, 0, 0, 1, - (region.left() - fLeftMargin * xlength), - (region.bottom() + fBottomMargin * ylength));

	double xscale =
		(viewport.right() - viewport.left() - iLeftMargin - iRightMargin) /
		(region.right() - region.left() + (fLeftMargin + fRightMargin) * xlength);
	double yscale = -
									(viewport.bottom() - viewport.top() - iTopMargin - iBottomMargin) /
									(region.bottom() - region.top() + (fTopMargin + fBottomMargin) * ylength);
	scale = QMatrix(xscale, 0, 0, yscale, 0, 0);

	translate2 = QMatrix(1, 0, 0, 1, viewport.left() + iLeftMargin, viewport.top() + iTopMargin);

	return translate1 * scale * translate2;
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraFit()
{
	QRect vp = viewport()->rect();
	m_drawnRegion = getRegion();
	viewport()->update();
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraMoveLeft()
{
	translate(- moveWidth(), 0);
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraMoveRight()
{
	translate(moveWidth(), 0);
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraMoveUp()
{
	translate(0, - moveWidth());
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraMoveDown()
{
	translate(0, moveWidth());
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraZoomIn()
{
	zoom(1.2, 1.2);
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraZoomOut()
{
	zoom(1. / 1.2, 1. / 1.2);
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraZoomInX()
{
	zoom(1.2, 1);
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraZoomOutX()
{
	zoom(1. / 1.2, 1);
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraZoomInY()
{
	zoom(1, 1.2);
}

void PreProcessorGridCrosssectionWindowGraphicsView::cameraZoomOutY()
{
	zoom(1, 1. / 1.2);
}

void PreProcessorGridCrosssectionWindowGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	int diffx = event->x() - m_oldPosition.x();
	int diffy = event->y() - m_oldPosition.y();

	if (m_mouseEventMode == meNormal || m_mouseEventMode == meMovePrepare) {
		m_mouseEventMode = meNormal;

		// find selected points newr the mouse cursor.
		QModelIndexList selectedPoints = m_parentWindow->m_selectionModel->selectedRows();

		Structured2DGrid* grid = m_parentWindow->grid();
		GridAttributeContainer* cond = grid->gridAttribute(m_parentWindow->condition());
		GridAttributeRealNodeContainer* cond2 = dynamic_cast<GridAttributeRealNodeContainer*>(cond);
		PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_parentWindow->conditionNodeDataItem()->parent()->parent());
		const QVector<vtkIdType>& selVertices = gItem->selectedVertices();

		QPointF mins(event->x() - 5, event->y() - 5);
		QPointF maxs(event->x() + 5, event->y() + 5);

		double distance = 0;

		if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirI) {
			for (unsigned int i = 0; i < grid->dimensionI(); ++i) {
				if (i > 0) {distance += getDistance(m_parentWindow->targetIndex(), i - 1, i);}
				vtkIdType index = grid->vertexIndex(i, m_parentWindow->targetIndex());
				if (! selVertices.contains(index)) {continue;}
				double v = cond2->value(index);
				QPointF p1 = m_matrix.map(QPointF(distance, v));
				if (p1.x() >= mins.x() &&
						p1.x() <= maxs.x() &&
						p1.y() >= mins.y() &&
						p1.y() <= maxs.y()) {
					m_mouseEventMode = meMovePrepare;
				}
			}
		} else if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirJ) {
			double offset = getDistance(m_parentWindow->targetIndex(), 0, grid->dimensionJ() - 1) * 0.5;
			for (unsigned int j = 0; j < grid->dimensionJ(); ++j) {
				if (j > 0) {distance += getDistance(m_parentWindow->targetIndex(), j - 1, j);}
				unsigned int index = grid->vertexIndex(m_parentWindow->targetIndex(), j);
				if (! selVertices.contains(index)) {continue;}
				double v = cond2->value(index);
				QPointF p1 = m_matrix.map(QPointF(offset - distance, v));
				if (p1.x() >= mins.x() &&
						p1.x() <= maxs.x() &&
						p1.y() >= mins.y() &&
						p1.y() <= maxs.y()) {
					m_mouseEventMode = meMovePrepare;
				}
			}
		}
		updateMouseCursor();
	} else if (m_mouseEventMode == meTranslating) {
		translate(diffx, diffy);
	} else if (m_mouseEventMode == meZooming) {
		double scaleX = 1 + diffx * 0.02;
		double scaleY = 1 - diffy * 0.02;
		if (scaleX < 0.5) {scaleX = 0.5;}
		if (scaleY < 0.5) {scaleY = 0.5;}
		if (scaleX > 2) {scaleX = 2;}
		if (scaleY > 2) {scaleY = 2;}
		zoom(scaleX, scaleY);
	} else if (m_mouseEventMode == meSelecting) {
		QPoint topLeft(qMin(m_rubberOrigin.x(), event->x()), qMin(m_rubberOrigin.y(), event->y()));
		QSize size(qAbs(m_rubberOrigin.x() - event->x()), qAbs(m_rubberOrigin.y() - event->y()));
		QRect rect(topLeft, size);
		m_rubberBand->setGeometry(rect);
		viewport()->update();
	} else if (m_mouseEventMode == meMove) {
		PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_parentWindow->conditionNodeDataItem()->parent()->parent());

		const QVector<vtkIdType>& selectedVertices = gItem->selectedVertices();
		Structured2DGrid* grid = m_parentWindow->grid();
		GridAttributeRealNodeContainer* cont = dynamic_cast<GridAttributeRealNodeContainer*>(grid->gridAttribute(m_parentWindow->condition()));
		double offset = getOffset(m_dragStartPoint, event->pos());
		QVector<double> before, after;
		before.reserve(selectedVertices.size());
		after.reserve(selectedVertices.size());
		for (int vid = 0; vid < selectedVertices.size(); ++vid) {
			before.append(cont->value(selectedVertices.at(vid)));
			after.append(cont->value(selectedVertices.at(vid)) + offset);
		}
		iRICUndoStack::instance().push(new Structured2DGridCrosssectionEditCommand(true, selectedVertices, before, after, m_parentWindow, cont, m_parentWindow->conditionNodeDataItem()));
		m_dragStartPoint = event->pos();
	}
	m_oldPosition = event->pos();
}

void PreProcessorGridCrosssectionWindowGraphicsView::mousePressEvent(QMouseEvent* event)
{
	switch (m_mouseEventMode) {
	case meNormal:
		if (event->modifiers() == Qt::ControlModifier) {
			switch (event->button()) {
			case Qt::LeftButton:
				// translate
				m_mouseEventMode = meTranslating;
				break;
			case Qt::MidButton:
				// zoom.
				m_mouseEventMode = meZooming;
				break;
			case Qt::RightButton:
				// do nothing.
				break;
			default:
				break;
			}
			m_oldPosition = event->pos();
			updateMouseCursor();
		} else {
			if (event->button() == Qt::LeftButton) {
				// start selecting.
				m_mouseEventMode = meSelecting;
				if (m_rubberBand == nullptr) {
					m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
				}
				m_rubberOrigin = event->pos();
				m_rubberBand->setGeometry(m_rubberOrigin.x(), m_rubberOrigin.y(), 0, 0);
				m_rubberBand->show();
			} else if (event->button() == Qt::RightButton) {
				m_dragStartPoint = event->pos();
			}
		}
		break;
	case meMovePrepare:
		if (event->button() == Qt::LeftButton) {
			// start dragging points.
			m_dragStartPoint = event->pos();
			m_mouseEventMode = meMove;
			updateMouseCursor();
		} else if (event->button() == Qt::RightButton) {
			m_dragStartPoint = event->pos();
		}
		break;
	default:
		break;
	}
}

double PreProcessorGridCrosssectionWindowGraphicsView::getOffset(const QPoint& start, const QPoint& end)
{
	QPointF startF(start);
	QPointF endF(end);
	QMatrix invMatrix = m_matrix.inverted();
	QPointF mappedStartF = invMatrix.map(startF);
	QPointF mappedEndF = invMatrix.map(endF);
	return mappedEndF.y() - mappedStartF.y();
}

void PreProcessorGridCrosssectionWindowGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	switch (m_mouseEventMode) {
	case meNormal:
	case meMovePrepare:
		if (event->button() == Qt::RightButton) {
			if (iRIC::isNear(m_dragStartPoint, event->pos())) {
				// show right-clicking menu.
				setupMenu();
				m_rightClickingMenu->move(event->globalPos());
				m_rightClickingMenu->show();
			}
		}
		break;
	case meTranslating:
	case meZooming:
		m_mouseEventMode = meNormal;
		// go back to normal mode.
		updateMouseCursor();
		break;
	case meSelecting:
		// finish selecting.
		m_rubberBand->hide();
		if (iRIC::isNear(m_rubberOrigin, event->pos())) {
			// press point and release point are too near.
			QPoint p1(event->pos().x() - 3, event->pos().y() - 3);
			QPoint p2(event->pos().x() + 3, event->pos().y() + 3);
			selectPoints(p1, p2);
		} else {
			// select the point inside the rubberband geometry.
			selectPoints(m_rubberOrigin, event->pos());
		}
		m_mouseEventMode = meNormal;
		// go back to normal mode.
		updateMouseCursor();
		break;
	case meMove:
		PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_parentWindow->conditionNodeDataItem()->parent()->parent());
		const QVector<vtkIdType>& selectedVertices = gItem->selectedVertices();
		Structured2DGrid* grid = m_parentWindow->grid();
		GridAttributeRealNodeContainer* cont = dynamic_cast<GridAttributeRealNodeContainer*>(grid->gridAttribute(m_parentWindow->condition()));
		double offset = getOffset(m_dragStartPoint, event->pos());
		QVector<double> before, after;
		before.reserve(selectedVertices.size());
		after.reserve(selectedVertices.size());
		for (int vid = 0; vid < selectedVertices.size(); ++vid) {
			before.append(cont->value(selectedVertices.at(vid)));
			after.append(cont->value(selectedVertices.at(vid)) + offset);
		}
		iRICUndoStack::instance().push(new Structured2DGridCrosssectionEditCommand(true, selectedVertices, before, after, m_parentWindow, cont, m_parentWindow->conditionNodeDataItem()));
		m_mouseEventMode = meMovePrepare;
		updateMouseCursor();
		break;
	}
}

void PreProcessorGridCrosssectionWindowGraphicsView::wheelEvent(QWheelEvent* event)
{
	if (event->orientation() == Qt::Horizontal) {return;}
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	if (numSteps > 0) {
		cameraZoomIn();
	} else {
		cameraZoomOut();
	}
}

void PreProcessorGridCrosssectionWindowGraphicsView::zoom(double scaleX, double scaleY)
{
	qreal drawnRegionCenterX = (m_drawnRegion.left() + m_drawnRegion.right()) * .5;
	qreal drawnRegionCenterY = (m_drawnRegion.top() + m_drawnRegion.bottom()) * .5;
	qreal xWidth = m_drawnRegion.right() - m_drawnRegion.left();
	qreal yWidth = m_drawnRegion.bottom() - m_drawnRegion.top();

	qreal newxWidth = xWidth / scaleX;
	qreal newyWidth = yWidth / scaleY;

	if (newxWidth < 1E-6) {newxWidth = 1E-6;}
	if (newyWidth < 1E-6) {newyWidth = 1E-6;}

	m_drawnRegion.setLeft(drawnRegionCenterX - newxWidth * 0.5);
	m_drawnRegion.setRight(drawnRegionCenterX + newxWidth * 0.5);
	m_drawnRegion.setTop(drawnRegionCenterY - newyWidth * 0.5);
	m_drawnRegion.setBottom(drawnRegionCenterY + newyWidth * 0.5);
	viewport()->update();
}

void PreProcessorGridCrosssectionWindowGraphicsView::translate(int x, int y)
{
	QWidget* w = viewport();
	QRect rect = w->rect();
	double xscale = (m_drawnRegion.width() / rect.width());
	double yscale = (m_drawnRegion.height() / rect.height());

	m_drawnRegion.setLeft(m_drawnRegion.left() - x * xscale);
	m_drawnRegion.setRight(m_drawnRegion.right() - x * xscale);
	m_drawnRegion.setTop(m_drawnRegion.top() + y * yscale);
	m_drawnRegion.setBottom(m_drawnRegion.bottom() + y * yscale);
	viewport()->update();
}

int PreProcessorGridCrosssectionWindowGraphicsView::moveWidth()
{
	QWidget* w = viewport();
	int stdW = qMax(w->width(), w->height());
	return stdW / 3;
}

void PreProcessorGridCrosssectionWindowGraphicsView::updateMouseCursor()
{
	switch (m_mouseEventMode) {
	case meNormal:
		setCursor(Qt::ArrowCursor);
		break;
	case meZooming:
		setCursor(m_zoomCursor);
		break;
	case meTranslating:
		setCursor(m_moveCursor);
		break;
	case meMove:
		setCursor(Qt::ClosedHandCursor);
		break;
	case meMovePrepare:
		setCursor(Qt::OpenHandCursor);
		break;
	}
}

void PreProcessorGridCrosssectionWindowGraphicsView::selectPoints(const QPoint& from, const QPoint& to)
{
	QPointF mins(qMin(from.x(), to.x()), qMax(from.y(), to.y()));
	QPointF maxs(qMax(from.x(), to.x()), qMin(from.y(), to.y()));

	QMatrix invMatrix = m_matrix.inverted();
	QPointF mappedMins = invMatrix.map(mins);
	QPointF mappedMaxs = invMatrix.map(maxs);
	int index = m_parentWindow->targetIndex();

	Structured2DGrid* grid = m_parentWindow->grid();
	PreProcessorGridAttributeNodeDataItem* item = m_parentWindow->conditionNodeDataItem();
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(item->parent()->parent());
	GridAttributeRealNodeContainer* cont = dynamic_cast<GridAttributeRealNodeContainer*>(grid->gridAttribute(m_parentWindow->condition()));
	QVector<vtkIdType> selectedVertices;
	if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirI) {
		double distance = 0;
		for (unsigned int i = 0; i < grid->dimensionI(); ++i) {
			if (i != 0) {
				distance += getDistance(index, i - 1, i);
			}
			double x = distance;
			double y = cont->value(grid->vertexIndex(i, index));
			if (x >= mappedMins.x() &&
					x <= mappedMaxs.x() &&
					y >= mappedMins.y() &&
					y <= mappedMaxs.y()) {
				selectedVertices.append(grid->vertexIndex(i, index));
			}
		}
		gItem->setSelectedVertices(selectedVertices);
	} else {
		double offset = getDistance(index, 0, grid->dimensionJ() - 1) * 0.5;
		double distance = 0;
		for (unsigned int j = 0; j < grid->dimensionJ(); ++j) {
			if (j != 0) {
				distance += getDistance(index, j - 1, j);
			}
			double x = offset - distance;
			double y = cont->value(grid->vertexIndex(index, j));
			if (x >= mappedMins.x() &&
					x <= mappedMaxs.x() &&
					y >= mappedMins.y() &&
					y <= mappedMaxs.y()) {
				selectedVertices.append(grid->vertexIndex(index, j));
			}
		}
		gItem->setSelectedVertices(selectedVertices);
	}
}

void PreProcessorGridCrosssectionWindowGraphicsView::informSelectedVerticesChanged(const QVector<vtkIdType>& vertices)
{
	QItemSelection selection;

	QModelIndex firstIndex;
	bool firstset = false;
	Structured2DGrid* grid = m_parentWindow->grid();
	for (int k = 0; k < vertices.count(); ++k) {
		int index = vertices.at(k);
		unsigned int i, j;
		grid->getIJIndex(index, &i, &j);
		if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirI) {
			if (j == m_parentWindow->targetIndex()) {
				selection.merge(QItemSelection(model()->index(i, 0), model()->index(i, 0)), QItemSelectionModel::Select);
				if (! firstset) {
					firstIndex = model()->index(i, 0);
					firstset = true;
				}
			}
		} else {
			if (i == m_parentWindow->targetIndex()) {
				selection.merge(QItemSelection(model()->index(j, 0), model()->index(j, 0)), QItemSelectionModel::Select);
				if (! firstset) {
					firstIndex = model()->index(j, 0);
					firstset = true;
				}
			}
		}
	}
	selectionModel()->clearSelection();
	selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void PreProcessorGridCrosssectionWindowGraphicsView::updateActionStatus()
{
	QModelIndexList rows = selectionModel()->selectedRows();
	if (rows.count() == 0) {
		m_editAction->setDisabled(true);
	} else {
		bool continuous = true;
		auto it = rows.begin();
		auto it2 = it;
		++it2;
		while (it2 != rows.end()) {
			continuous = continuous && (it2->row() == it->row() + 1);
			++it; ++it2;
		}
		m_editAction->setEnabled(continuous);
	}
}

void PreProcessorGridCrosssectionWindowGraphicsView::moveSelectedRows()
{
	iRICMainWindowInterface* mw = m_parentWindow->projectDataItem()->iricMainWindow();
	if (mw->isSolverRunning()) {
		mw->warnSolverRunning();
		return;
	}
	Structured2DGrid* grid = m_parentWindow->grid();
	GridAttributeContainer* cont = grid->gridAttribute(m_parentWindow->condition());
	GridAttributeEditDialog* dialog = cont->gridAttribute()->editDialog(this);
	dialog->setWindowTitle(tr("Edit %1").arg(cont->gridAttribute()->caption()));
	dialog->setLabel(QString(tr("Input the new value of %1 at the selected grid nodes.")).arg(cont->gridAttribute()->caption()));
	QVector<vtkIdType> targets;
	QModelIndexList selIndices = selectionModel()->selectedIndexes();
	for (QModelIndex index : selIndices) {
		vtkIdType targetindex;
		if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirI) {
			targetindex = grid->vertexIndex(index.row(), m_parentWindow->targetIndex());
		} else {
			targetindex = grid->vertexIndex(m_parentWindow->targetIndex(), index.row());
		}
		targets.append(targetindex);
	}
	dialog->scanAndSetDefault(cont, targets);

	if (QDialog::Accepted == dialog->exec()) {
		PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(m_parentWindow->projectDataItem()->conditionNodeDataItem()->parent()->parent());
		dialog->applyValue(cont, targets, grid->vtkGrid()->GetPointData(), gItem);
	}
	delete dialog;
}

double PreProcessorGridCrosssectionWindowGraphicsView::getDistance(int index, int startIndex, int endIndex)
{
	Structured2DGrid* grid = m_parentWindow->grid();
	double distance = 0;
	if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirI) {
		QPointF p0 = grid->vertex(startIndex, index);
		for (int i = startIndex; i <= endIndex; ++i) {
			QPointF p1 = grid->vertex(i, index);
			distance += iRIC::length(p1 - p0);
			p0 = p1;
		}
	} else if (m_parentWindow->targetDirection() == PreProcessorGridCrosssectionWindow::dirJ) {
		QPointF p0 = grid->vertex(index, startIndex);
		for (int j = startIndex; j <= endIndex; ++j) {
			QPointF p1 = grid->vertex(index, j);
			distance += iRIC::length(p1 - p0);
			p0 = p1;
		}
	}
	return distance;
}
