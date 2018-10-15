#include "gridcreatingconditionriversurveyinterface.h"
#include "geodatarivercrosssection.h"
#include "geodatarivercrosssectionaltitudemovedialog.h"
#include "geodatariverpathpoint.h"
#include "geodatariversurvey.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowgraphicsview.h"

#include <guicore/misc/qundocommandhelper.h>
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
#include <QTextStream>
#include <QWheelEvent>

#include <cmath>

GeoDataRiverSurveyCrosssectionWindowGraphicsView::GeoDataRiverSurveyCrosssectionWindowGraphicsView(QWidget* w) :
	QAbstractItemView {w},
	fLeftMargin {0.2},
	fRightMargin {0.2},
	fTopMargin {0.2},
	fBottomMargin {0.2},
	m_mouseEventMode {meNormal},
	m_rubberBand {nullptr},
	m_rightClickingMenu {nullptr},
	m_gridMode {false}
{
	// Set cursors for mouse view change events.
	m_zoomPixmap = QPixmap(":/libs/guibase/images/cursorZoom.png");
	m_movePixmap = QPixmap(":/libs/guibase/images/cursorMove.png");

	m_zoomCursor = QCursor(m_zoomPixmap);
	m_moveCursor = QCursor(m_movePixmap);
	setMouseTracking(true);
	setupActions();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::setupActions()
{
	m_activateAction = new QAction(tr("&Activate"), this);
	connect(m_activateAction, SIGNAL(triggered()), this, SLOT(activateSelectedRows()));
	m_inactivateAction = new QAction(tr("&Inactivate"), this);
	connect(m_inactivateAction, SIGNAL(triggered()), this, SLOT(inactivateSelectedRows()));
	m_moveAction = new QAction(tr("&Move"), this);
	connect(m_moveAction, SIGNAL(triggered()), this, SLOT(moveSelectedRows()));
	m_activateAction->setEnabled(false);
	m_inactivateAction->setEnabled(false);
	m_moveAction->setEnabled(false);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::setupMenu()
{
	if (m_rightClickingMenu == nullptr) {
		m_rightClickingMenu = new QMenu(this);
		m_rightClickingMenu->addAction(m_activateAction);
		m_rightClickingMenu->addAction(m_inactivateAction);

		m_rightClickingMenu->addSeparator();
		QMenu* submenu = m_rightClickingMenu->addMenu(tr("Inactivate using &water elevation"));
		submenu->addAction(m_parentWindow->inactivateByWEOnlyThisAction());
		submenu->addAction(m_parentWindow->inactivateByWEAllAction());

		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(m_moveAction);
		m_rightClickingMenu->addAction(m_parentWindow->deleteAction());
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/)
{
	viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::paintEvent(QPaintEvent* /*e*/)
{
	QPainter painter(viewport());
	QRect vp = painter.viewport();
	QMatrix matrix = getMatrix(vp);
	m_matrix = matrix;
	painter.setRenderHint(QPainter::Antialiasing);
	// Draw scales.
	drawScales(painter, matrix);
	// draw water surface.
	drawWaterSurfaceElevation(m_parentWindow->target(), painter, matrix);

	if (! m_gridMode) {
		// draw lines.
		// black lines
		QPen pen = QPen(Qt::black, 1);
		painter.setPen(pen);

		for (int i = 0; i < m_parentWindow->riverPathPoints().count(); ++i) {
			GeoDataRiverPathPoint* p = m_parentWindow->riverPathPoints().at(i);
			if (p == nullptr) {continue;}
			bool enabled = m_parentWindow->riverSurveyEnables().at(i);
			if (! enabled) {continue;}
			QColor c = m_parentWindow->riverSurveyColors().at(i);
			drawLine(p, c, painter);
		}

		// draw circles.
		drawCircle(painter);
		// draw selected circles.
		drawSelectionCircle(painter);
	} else {
		// draw black lines.
		drawLine(m_parentWindow->gridCreatingConditionPoint(), Qt::black, painter);
		// draw yellow squares.
		drawSquare(painter);
		// draw selected yellow squares.
		drawSelectionSquare(painter);
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawLine(GeoDataRiverPathPoint* point, const QColor& color, QPainter& painter)
{
	if (point == nullptr) {return;}
	GeoDataRiverCrosssection& cross = point->crosssection();
	bool first = true;
	QPointF oldpoint, newpoint;
	painter.setPen(color);
	for (const GeoDataRiverCrosssection::Altitude& alt : cross.AltitudeInfo()) {
		if (! alt.active()) {continue;}
		newpoint = m_matrix.map(QPointF(alt.position(), alt.height()));
		if (! first) {
			painter.drawLine(oldpoint, newpoint);
		}
		oldpoint = newpoint;
		first = false;
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawCircle(QPainter& painter)
{
	if (m_parentWindow->target() == nullptr) {return;}

	QPen pen(Qt::black, 1, Qt::SolidLine);
	QBrush activeBrush(Qt::red, Qt::SolidPattern);
	QBrush inactiveBrush(Qt::green, Qt::SolidPattern);
	QBrush fixBrush(Qt::gray, Qt::SolidPattern);

	GeoDataRiverCrosssection& cross = m_parentWindow->target()->crosssection();
	painter.setPen(pen);
	const GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	for (const GeoDataRiverCrosssection::Altitude& alt : alist) {
		if (alt.active()) {
			painter.setBrush(activeBrush);
		} else {
			painter.setBrush(inactiveBrush);
		}
		QPointF point = m_matrix.map(QPointF(alt.position(), alt.height()));
		QRectF r(point.x() - ellipseR, point.y() - ellipseR, ellipseR * 2, ellipseR * 2);
		painter.drawEllipse(r);
	}
	painter.setBrush(fixBrush);

	// draw fixed points.
	if (cross.fixedPointLSet()) {
		int lindex = cross.fixedPointLIndex();
		GeoDataRiverCrosssection::Altitude alt = alist.at(lindex);
		QPointF point = m_matrix.map(QPointF(alt.position(), alt.height()));
		QRectF r(point.x() - selectedEllipseR, point.y() - selectedEllipseR, selectedEllipseR * 2, selectedEllipseR * 2);
		painter.drawEllipse(r);
	}
	if (cross.fixedPointRSet()) {
		int lindex = cross.fixedPointRIndex();
		GeoDataRiverCrosssection::Altitude alt = alist.at(lindex);
		QPointF point = m_matrix.map(QPointF(alt.position(), alt.height()));
		QRectF r(point.x() - selectedEllipseR, point.y() - selectedEllipseR, selectedEllipseR * 2, selectedEllipseR * 2);
		painter.drawEllipse(r);
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawSelectionCircle(QPainter& painter)
{
	if (m_parentWindow->target() == nullptr) {return;}

	QPen pen(Qt::black, 1, Qt::SolidLine);
	QBrush activeBrush(Qt::red, Qt::SolidPattern);
	QBrush inactiveBrush(Qt::green, Qt::SolidPattern);

	GeoDataRiverCrosssection& cross = m_parentWindow->target()->crosssection();
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	painter.setPen(pen);
	QSet<int> drawnRows;
	for (const QModelIndex& index : selectionModel()->selectedIndexes()) {
		if (drawnRows.contains(index.row())) {continue;}
		const GeoDataRiverCrosssection::Altitude& alt = alist.at(index.row());
		if (alt.active()) {
			painter.setBrush(activeBrush);
		} else {
			painter.setBrush(inactiveBrush);
		}
		QPointF point = m_matrix.map(QPointF(alt.position(), alt.height()));
		QRectF r(point.x() - selectedEllipseR, point.y() - selectedEllipseR, selectedEllipseR * 2, selectedEllipseR * 2);
		painter.drawEllipse(r);
		drawnRows.insert(index.row());
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawSquare(QPainter& painter)
{
	QPen pen(Qt::black, 1, Qt::SolidLine);
	QBrush activeBrush(Qt::yellow, Qt::SolidPattern);
	QBrush inactiveBrush(Qt::blue, Qt::SolidPattern);

	GeoDataRiverPathPoint* blackpoint = m_parentWindow->gridCreatingConditionPoint();
	if (blackpoint == nullptr) {return;}
	GeoDataRiverCrosssection& cross = blackpoint->crosssection();
	painter.setPen(pen);
	painter.setBrush(inactiveBrush);

	// left bank
	GeoDataRiverCrosssection::Altitude leftbank = cross.leftBank(true);
	QPointF point = m_matrix.map(QPointF(leftbank.position(), leftbank.height()));
	QRectF r(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
	painter.drawRect(r);

	// right bank
	GeoDataRiverCrosssection::Altitude rightbank = cross.rightBank(true);
	point = m_matrix.map(QPointF(rightbank.position(), rightbank.height()));
	r = QRectF(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
	painter.drawRect(r);

	// river center
	double height = blackpoint->lXSec()->interpolate(0).height();
	point = m_matrix.map(QPointF(0, height));
	r = QRectF(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
	painter.drawRect(r);

	painter.setBrush(activeBrush);
	// control points between rivercenter and left bank
	for (double v : blackpoint->CenterToLeftCtrlPoints) {
		GeoDataRiverCrosssection::Altitude alt = blackpoint->lXSec()->interpolate(v);
		point = m_matrix.map(QPointF(alt.position(), alt.height()));
		r = QRectF(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
		painter.drawRect(r);
	}
	// control points between rivercenter and right bank
	for (double v : blackpoint->CenterToRightCtrlPoints) {
		GeoDataRiverCrosssection::Altitude alt = blackpoint->rXSec()->interpolate(v);
		point = m_matrix.map(QPointF(alt.position(), alt.height()));
		r = QRectF(point.x() - squareR, point.y() - squareR, squareR * 2, squareR * 2);
		painter.drawRect(r);
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawSelectionSquare(QPainter& painter)
{
	GeoDataRiverPathPoint* blackpoint = m_parentWindow->gridCreatingConditionPoint();
	if (blackpoint == nullptr) {return;}
	std::list<CtrlPointSelectionInfo>& sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();

	for (const CtrlPointSelectionInfo& info : sel) {
		if (info.Point == blackpoint) {
			if (info.Position == GeoDataRiverPathPoint::pposCenterToLeft) {
				double v = blackpoint->CenterToLeftCtrlPoints[info.Index];
				GeoDataRiverCrosssection::Altitude alt = blackpoint->lXSec()->interpolate(v);
				QPointF point = m_matrix.map(QPointF(alt.position(), alt.height()));
				QRectF r(point.x() - selectedSquareR, point.y() - selectedSquareR, selectedSquareR * 2, selectedSquareR * 2);
				painter.drawRect(r);
			} else if (info.Position == GeoDataRiverPathPoint::pposCenterToRight) {
				double v = blackpoint->CenterToRightCtrlPoints[info.Index];
				GeoDataRiverCrosssection::Altitude alt = blackpoint->rXSec()->interpolate(v);
				QPointF point = m_matrix.map(QPointF(alt.position(), alt.height()));
				QRectF r(point.x() - selectedSquareR, point.y() - selectedSquareR, selectedSquareR * 2, selectedSquareR * 2);
				painter.drawRect(r);
			}
		}
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawScales(QPainter& painter, const QMatrix& matrix)
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

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::drawWaterSurfaceElevation(GeoDataRiverPathPoint* point, QPainter& painter, const QMatrix& matrix)
{
	if (point == nullptr) {return;}
	if (! point->waterSurfaceElevationSpecified()) {return;}
	double ele = point->waterSurfaceElevationValue();

	QWidget* w = viewport();
	QMatrix invMatrix = matrix.inverted();
	QPointF mins, maxs;
	mins = invMatrix.map(QPointF(0, w->height()));
	maxs = invMatrix.map(QPointF(w->width(), 0));

	QPointF from = matrix.map(QPointF(0, ele));
	from.setX(0);
	QPointF to(w->width(), from.y());

	QPen oldPen = painter.pen();
	QPen pen(Qt::blue);
	painter.setPen(pen);
	painter.drawLine(from, to);

	QPointF points[6];
	points[0] = QPointF(w->width() * 0.5, from.y() - 5);
	points[1] = QPointF(w->width() * 0.5 + 5, from.y() - 15);
	points[2] = points[1];
	points[3] = QPointF(w->width() * 0.5 - 5, from.y() - 15);
	points[4] = points[3];
	points[5] = points[0];

	painter.drawLines(points, 3);

	QRectF fontRect;
	fontRect = QRectF(w->width() * 0.5 + 10, from.y() - 20, 40, 20);
	int precision = 2;
	double limit = 1;
	for (int i = 0; i < 5; ++i) {
		if (ele <= -limit || limit <= ele) {++ precision;}
		limit *= 10;
	}

	painter.drawText(fontRect, Qt::AlignLeft | Qt::AlignVCenter, QString::number(ele, 'g', precision));

	painter.setPen(oldPen);
}

QRectF GeoDataRiverSurveyCrosssectionWindowGraphicsView::getRegion()
{
	QRectF ret(0., 0., 0., 0.);
	bool first = true;
	for (int i = 0; i < m_parentWindow->riverPathPoints().count(); ++i) {
		GeoDataRiverPathPoint* p = m_parentWindow->riverPathPoints()[i];
		if (p == nullptr) {continue;}
		GeoDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
		for (int j = 0; j < alist.size(); ++j) {
			GeoDataRiverCrosssection::Altitude alt = alist.at(j);
			if (first || alt.position() < ret.left()) {ret.setLeft(alt.position());}
			if (first || alt.position() > ret.right()) {ret.setRight(alt.position());}
			if (first || alt.height() < ret.top()) {ret.setTop(alt.height());}
			if (first || alt.height() > ret.bottom()) {ret.setBottom(alt.height());}
			first = false;
		}
	}

	if (ret.left() == ret.right()) {
		double center = ret.left();
		ret.setLeft(center - 0.5);
		ret.setRight(center + 0.5);
	}
	if (ret.top() == ret.bottom()) {
		double center = ret.top();
		ret.setTop(center - 0.5);
		ret.setBottom(center + 0.5);
	}
	return ret;
}

QMatrix GeoDataRiverSurveyCrosssectionWindowGraphicsView::getMatrix(QRect& viewport)
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

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraFit()
{
	m_drawnRegion = getRegion();
	viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraMoveLeft()
{
	translate(- moveWidth(), 0);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraMoveRight()
{
	translate(moveWidth(), 0);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraMoveUp()
{
	translate(0, - moveWidth());
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraMoveDown()
{
	translate(0, moveWidth());
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomIn()
{
	zoom(1.2, 1.2);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomOut()
{
	zoom(1. / 1.2, 1. / 1.2);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomInX()
{
	zoom(1.2, 1);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomOutX()
{
	zoom(1. / 1.2, 1);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomInY()
{
	zoom(1, 1.2);
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::cameraZoomOutY()
{
	zoom(1, 1. / 1.2);
}


class GeoDataRiverSurvey::MouseEditCrosssectionCommand : public QUndoCommand
{
public:
	MouseEditCrosssectionCommand(GeoDataRiverPathPoint* p, const GeoDataRiverCrosssection::AltitudeList& after, const GeoDataRiverCrosssection::AltitudeList& before, GeoDataRiverSurveyCrosssectionWindow* w, GeoDataRiverSurvey* rs, bool dragging = false) :
		QUndoCommand {GeoDataRiverSurveyCrosssectionWindowGraphicsView::tr("Altitude Points Move")}
	{
		m_point = p;
		m_before = before;
		m_after = after;
		m_window = w;
		m_rs = rs;
		m_dragging = dragging;
	}
	void redo() {
		m_point->crosssection().AltitudeInfo() = m_after;
		m_point->updateXSecInterpolators();
		m_point->updateRiverShapeInterpolators();
		if (m_dragging) {
			m_window->updateView();
		} else {
			m_rs->updateShapeData();
			m_rs->renderGraphicsView();
			m_rs->updateCrossectionWindows();
		}
	}
	void undo() {
		m_point->crosssection().AltitudeInfo() = m_before;
		m_point->updateXSecInterpolators();
		m_point->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
	int id() const {
		return iRIC::generateCommandId("GeoDataRiverSurveyCrosssectionDragEdit");
	}
	bool mergeWith(const QUndoCommand* other) {
		const MouseEditCrosssectionCommand* comm = dynamic_cast<const MouseEditCrosssectionCommand*>(other);
		if (comm == nullptr) {return false;}
		if (m_point != comm->m_point) {return false;}
		if (! m_dragging) {return false;}
		m_after = comm->m_after;
		m_dragging = comm->m_dragging;
		return true;
	}
private:
	bool m_dragging;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverCrosssection::AltitudeList m_before;
	GeoDataRiverCrosssection::AltitudeList m_after;
	GeoDataRiverSurveyCrosssectionWindow* m_window;
	GeoDataRiverSurvey* m_rs;
};

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	int diffx = event->x() - m_oldPosition.x();
	int diffy = event->y() - m_oldPosition.y();

	if (m_mouseEventMode == meNormal || m_mouseEventMode == meMovePrepare) {
		m_mouseEventMode = meNormal;
		if (m_gridMode) {
			// find selected points near the mouse cursor.
			std::list<CtrlPointSelectionInfo> sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
			QPointF mins(event->x() - 5, event->y() + 5);
			QPointF maxs(event->x() + 5, event->y() - 5);
			QMatrix invMatrix = m_matrix.inverted();
			QPointF mappedMins = invMatrix.map(mins);
			QPointF mappedMaxs = invMatrix.map(maxs);
			if (continuousGridSelection()) {
				CtrlPointSelectionInfo info = sel.front();
				if (info.Point == m_parentWindow->gridCreatingConditionPoint()) {
					if (info.Position == GeoDataRiverPathPoint::pposCenterToLeft) {
						for (auto it = sel.begin(); it != sel.end(); ++it) {
							info = *it;
							GeoDataRiverCrosssection::Altitude alt = info.Point->lXSec()->interpolate(info.Point->CtrlPoints(info.Position).at(info.Index));
							if (alt.position() >= mappedMins.x() &&
									alt.position() <= mappedMaxs.x() &&
									alt.height() >= mappedMins.y() &&
									alt.height() <= mappedMaxs.y()) {
								// one of the selected points is near enough to the mouse pointer.
								m_mouseEventMode = meMovePrepare;
							}
						}
					} else if (info.Position == GeoDataRiverPathPoint::pposCenterToRight) {
						for (auto it = sel.begin(); it != sel.end(); ++it) {
							info = *it;
							GeoDataRiverCrosssection::Altitude alt = info.Point->rXSec()->interpolate(info.Point->CtrlPoints(info.Position).at(info.Index));
							if (alt.position() >= mappedMins.x() &&
									alt.position() <= mappedMaxs.x() &&
									alt.height() >= mappedMins.y() &&
									alt.height() <= mappedMaxs.y()) {
								// one of the selected points is near enough to the mouse pointer.
								m_mouseEventMode = meMovePrepare;
							}
						}
					}
				}
			}
		} else {
			// find selected points near the mouse cursor.
			QModelIndexList selectedPoints = m_parentWindow->selectionModel()->selectedRows();
			GeoDataRiverPathPoint* p = m_parentWindow->target();
			GeoDataRiverCrosssection::AltitudeList& alist = p->crosssection().AltitudeInfo();
			QPointF mins(event->x() - 5, event->y() + 5);
			QPointF maxs(event->x() + 5, event->y() - 5);
			QMatrix invMatrix = m_matrix.inverted();
			QPointF mappedMins = invMatrix.map(mins);
			QPointF mappedMaxs = invMatrix.map(maxs);
			if (continuousSelection()) {
				for (auto it = selectedPoints.begin(); it != selectedPoints.end(); ++it) {
					int index = it->row();
					GeoDataRiverCrosssection::Altitude& alt = alist[index];
					if (alt.position() >= mappedMins.x() &&
							alt.position() <= mappedMaxs.x() &&
							alt.height() >= mappedMins.y() &&
							alt.height() <= mappedMaxs.y()) {
						// one of the selected points is near enough to the mouse pointer.
						m_mouseEventMode = meMovePrepare;
					}
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
		if (m_gridMode) {
			std::list<CtrlPointSelectionInfo> sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
			double offset = getGridCtrlPointOffset(m_dragStartPoint, event->pos());
			iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointMoveCommand(true, offset, m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()));
			/*
						std::list<CtrlPointSelectionInfo> sel =  m_parentWindow->m_targetRiverSurvey->gridCreatingCondition()->selectedCtrlPointInfoList();
						CtrlPointSelectionInfo info = sel.front();
						std::list<CtrlPointSelectionInfo> newSel = m_parentWindow->m_targetRiverSurvey->gridCreatingCondition()->selectedCtrlPointInfoList();
						updateGridCtrlPointList(newSel, m_dragStartPoint, event->pos());
						iRICUndoStack::instance().push(new GeoDataRiverSurveyCrosssectionDragEditCommand(m_parentWindow->m_blackLinePoint, newAltitudeList, m_oldAltitudeList, m_parentWindow, m_parentWindow->m_targetRiverSurvey, true));
			*/

			/*
						GeoDataRiverCrosssection::AltitudeList newAltitudeList = m_oldAltitudeList;
						updateAltitudeList(newAltitudeList, m_dragStartPoint, event->pos());
						iRICUndoStack::instance().push(new GeoDataRiverSurveyCrosssectionDragEditCommand(m_parentWindow->m_blackLinePoint, newAltitudeList, m_oldAltitudeList, m_parentWindow, m_parentWindow->m_targetRiverSurvey, true));
			*/
		} else {
			GeoDataRiverCrosssection::AltitudeList newAltitudeList = m_oldAltitudeList;
			updateAltitudeList(newAltitudeList, m_dragStartPoint, event->pos());
			iRICUndoStack::instance().push(new GeoDataRiverSurvey::MouseEditCrosssectionCommand(m_parentWindow->target(), newAltitudeList, m_oldAltitudeList, m_parentWindow, m_parentWindow->targetRiverSurvey(), true));
		}
	}
	m_oldPosition = event->pos();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::mousePressEvent(QMouseEvent* event)
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
			default:
				// do nothing.
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
			if (m_gridMode) {
				// start dragging points.
				m_dragStartPoint = event->pos();
				inspectGridLimits(&m_dragLimit.min, &m_dragLimit.max);
			} else {
				m_dragStartPoint = event->pos();
				m_oldAltitudeList = m_parentWindow->target()->crosssection().AltitudeInfo();
				inspectLimits(&m_dragLimit.minSet, &m_dragLimit.min, &m_dragLimit.maxSet, &m_dragLimit.max);
			}
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

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::mouseReleaseEvent(QMouseEvent* event)
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
		if (m_gridMode) {
			// implement this!
			std::list<CtrlPointSelectionInfo> sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
			double offset = getGridCtrlPointOffset(m_dragStartPoint, event->pos());
			iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointMoveCommand(false, offset, m_parentWindow->targetRiverSurvey()->gridCreatingCondition()));
			m_mouseEventMode = meNormal;
		} else {
			GeoDataRiverCrosssection::AltitudeList newAltitudeList = m_oldAltitudeList;
			updateAltitudeList(newAltitudeList, m_dragStartPoint, event->pos());
			iRICUndoStack::instance().push(new GeoDataRiverSurvey::MouseEditCrosssectionCommand(m_parentWindow->target(), newAltitudeList, m_oldAltitudeList, m_parentWindow, m_parentWindow->targetRiverSurvey(), false));
			m_mouseEventMode = meNormal;
		}
		updateMouseCursor();
		break;
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::wheelEvent(QWheelEvent* event)
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

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::zoom(double scaleX, double scaleY)
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

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::translate(int x, int y)
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

int GeoDataRiverSurveyCrosssectionWindowGraphicsView::moveWidth()
{
	QWidget* w = viewport();
	int stdW = qMax(w->width(), w->height());
	return stdW / 3;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::updateMouseCursor()
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
	default:
		break;
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::selectPoints(const QPoint& from, const QPoint& to)
{
	QPointF mins(qMin(from.x(), to.x()), qMax(from.y(), to.y()));
	QPointF maxs(qMax(from.x(), to.x()), qMin(from.y(), to.y()));

	QMatrix invMatrix = m_matrix.inverted();
	QPointF mappedMins = invMatrix.map(mins);
	QPointF mappedMaxs = invMatrix.map(maxs);

	if (m_gridMode) {
		GeoDataRiverPathPoint* blackpoint = m_parentWindow->gridCreatingConditionPoint();
		std::list<CtrlPointSelectionInfo>& sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
		sel.clear();
		CtrlPointSelectionInfo selInfo;
		selInfo.Point = blackpoint;

		// left side
		for (int i = 0; i < blackpoint->CenterToLeftCtrlPoints.count(); ++i) {
			double v = blackpoint->CenterToLeftCtrlPoints.at(i);
			GeoDataRiverCrosssection::Altitude alt = blackpoint->lXSec()->interpolate(v);
			if (alt.position() >= mappedMins.x() &&
					alt.position() <= mappedMaxs.x() &&
					alt.height() >= mappedMins.y() &&
					alt.height() <= mappedMaxs.y()) {
				selInfo.Position = GeoDataRiverPathPoint::pposCenterToLeft;
				selInfo.Index = i;
				sel.push_back(selInfo);
			}
		}
		// right side
		for (int i = 0; i < blackpoint->CenterToRightCtrlPoints.count(); ++i) {
			double v = blackpoint->CenterToRightCtrlPoints.at(i);
			GeoDataRiverCrosssection::Altitude alt = blackpoint->rXSec()->interpolate(v);
			if (alt.position() >= mappedMins.x() &&
					alt.position() <= mappedMaxs.x() &&
					alt.height() >= mappedMins.y() &&
					alt.height() <= mappedMaxs.y()) {
				selInfo.Position = GeoDataRiverPathPoint::pposCenterToRight;
				selInfo.Index = i;
				sel.push_back(selInfo);
			}
		}
		m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->updateShapeData();
		m_parentWindow->gridCreatingConditionRiverSurvey()->renderGraphicsView();
	} else {
		if (m_parentWindow->target() == nullptr) {return;}
		QItemSelection selection;

		GeoDataRiverCrosssection& cross = m_parentWindow->target()->crosssection();
		GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
		int row = 0;
		QModelIndex firstIndex;
		bool firstset = false;
		for (auto it = alist.begin(); it != alist.end(); ++it) {
			GeoDataRiverCrosssection::Altitude alt = *it;
			if (alt.position() >= mappedMins.x() &&
					alt.position() <= mappedMaxs.x() &&
					alt.height() >= mappedMins.y() &&
					alt.height() <= mappedMaxs.y()) {
				selection.merge(QItemSelection(model()->index(row, 0), model()->index(row, 2)), QItemSelectionModel::Select);
				if (! firstset) {
					firstIndex = model()->index(row, 0);
					firstset = true;
				}
			}
			++row;
		}
		selectionModel()->clearSelection();
		selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
		if (firstset) {
			m_parentWindow->tableView()->scrollTo(firstIndex);
		}
	}
	viewport()->update();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::updateActionStatus()
{
	QModelIndexList rows = selectionModel()->selectedRows();
	m_activateAction->setEnabled(rows.count() > 0);
	m_inactivateAction->setEnabled(rows.count() > 0);
	if (rows.count() == 0) {
		m_moveAction->setDisabled(true);
	} else {
		bool continuous = true;
		auto it = rows.begin();
		auto it2 = it;
		++it2;
		while (it2 != rows.end()) {
			continuous = continuous && (it2->row() == it->row() + 1);
			++it; ++it2;
		}
		m_moveAction->setEnabled(continuous);
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::activateSelectedRows()
{
	if (m_parentWindow->target() == nullptr) {return;}
	QModelIndexList rows = selectionModel()->selectedRows();
	GeoDataRiverCrosssection& cross = m_parentWindow->target()->crosssection();
	GeoDataRiverCrosssection::AltitudeList before, after;
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	before = alist;
	for (auto it = rows.begin(); it != rows.end(); ++it) {
		QModelIndex index = *it;
		alist[index.row()].setActive(true);
	}
	after = alist;
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Inactivate Elevation Points"), m_parentWindow->target(), after, before, m_parentWindow, m_parentWindow->targetRiverSurvey()));
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::inactivateSelectedRows()
{
	if (m_parentWindow->target() == nullptr) {return;}
	QModelIndexList rows = selectionModel()->selectedRows();
	GeoDataRiverCrosssection& cross = m_parentWindow->target()->crosssection();
	GeoDataRiverCrosssection::AltitudeList before, after;
	GeoDataRiverCrosssection::AltitudeList& alist = cross.AltitudeInfo();
	before = alist;
	std::vector<int> indices;
	for (auto it = rows.begin(); it != rows.end(); ++it) {
		QModelIndex index = *it;
		indices.push_back(index.row());
	}
	if (! m_parentWindow->canInactivateSelectedRows(cross, indices)) {
		alist = before;
		return;
	}
	after = alist;
	iRICUndoStack::instance().push(new GeoDataRiverSurvey::EditCrosssectionCommand(false, tr("Inactivate Elevation Points"), m_parentWindow->target(), after, before, m_parentWindow, m_parentWindow->targetRiverSurvey()));
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::moveSelectedRows()
{
	if (m_parentWindow->target() == nullptr) {return;}
	QModelIndexList rows = selectionModel()->selectedRows();
	int from = rows.front().row();
	int to = rows.back().row();
	GeoDataRiverCrosssectionAltitudeMoveDialog dialog(m_parentWindow->target(), from, to, m_parentWindow->targetRiverSurvey(), m_parentWindow, this);
	dialog.exec();
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::updateAltitudeList(GeoDataRiverCrosssection::AltitudeList& alist, const QPoint& start, const QPoint& end)
{
	QModelIndexList selectedPoints = m_parentWindow->selectionModel()->selectedRows();
	QPointF startF(start);
	QPointF endF(end);
	QMatrix invMatrix = m_matrix.inverted();
	QPointF mappedStartF = invMatrix.map(startF);
	QPointF mappedEndF = invMatrix.map(endF);
	double xoffset = mappedEndF.x() - mappedStartF.x();
	if (m_dragLimit.minSet && xoffset < m_dragLimit.min) {
		xoffset = m_dragLimit.min;
	}
	if (m_dragLimit.maxSet && xoffset > m_dragLimit.max) {
		xoffset = m_dragLimit.max;
	}
	double yoffset = mappedEndF.y() - mappedStartF.y();
	for (auto it = selectedPoints.begin(); it != selectedPoints.end(); ++it) {
		int index = it->row();
		GeoDataRiverCrosssection::Altitude& alt = alist[index];
		alt.setPosition(alt.position() + xoffset);
		alt.setHeight(alt.height() + yoffset);
	}
}

double GeoDataRiverSurveyCrosssectionWindowGraphicsView::getGridCtrlPointOffset(const QPoint& start, const QPoint& end)
{
	QPointF startF(start);
	QPointF endF(end);
	QMatrix invMatrix = m_matrix.inverted();
	QPointF mappedStartF = invMatrix.map(startF);
	QPointF mappedEndF = invMatrix.map(endF);
	double xoffset = mappedEndF.x() - mappedStartF.x();
	std::list<CtrlPointSelectionInfo> sel = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
	CtrlPointSelectionInfo info = sel.front();
	if (info.Position == GeoDataRiverPathPoint::pposCenterToLeft) {
		xoffset /= (info.Point->lXSec()->interpolate(1).position());
	} else if (info.Position == GeoDataRiverPathPoint::pposCenterToRight) {
		xoffset /= (info.Point->rXSec()->interpolate(1).position());
	}
	if (xoffset < m_dragLimit.min) {
		xoffset = m_dragLimit.min;
	}
	if (xoffset > m_dragLimit.max) {
		xoffset = m_dragLimit.max;
	}
	return xoffset;
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::inspectLimits(bool* minlimit, double* min, bool* maxlimit, double* max)
{
	QModelIndexList selectedPoints = m_parentWindow->selectionModel()->selectedRows();
	GeoDataRiverCrosssection::AltitudeList& alist = m_parentWindow->target()->crosssection().AltitudeInfo();

	// First, specify that both min and max limit is set.
	*minlimit = true;
	*maxlimit = true;

	int left = selectedPoints.first().row() - 1;
	int right = selectedPoints.last().row() + 1;

	if (left == - 1) {
		left = 0;
		*minlimit = false;
	}
	if (right == alist.size()) {
		right = static_cast<int>(alist.size()) - 1;
		*maxlimit = false;
	}
	double min1, min2, max1, max2;
	min1 = alist[left].position() - alist[selectedPoints.first().row()].position();
	max1 = alist[right].position() - alist[selectedPoints.last().row()].position();
	min2 = 0 - alist.back().position();
	max2 = 0 - alist.front().position();
	if (*minlimit) {
		*max = max1;
		if (*maxlimit) {
			*min = min1;
		} else {
			*min = std::max(min1, min2);
		}
	} else {
		*min = min2;
		if (*maxlimit) {
			*max = std::min(max1, max2);
		} else {
			*max = max2;

			*minlimit = true;
			*maxlimit = true;
		}
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::inspectGridLimits(double* min, double* max)
{
	std::list<CtrlPointSelectionInfo>& list = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
	std::list<CtrlPointSelectionInfo> tmplist;
	for (auto it = list.begin(); it != list.end(); ++it) {
		if (it->Index < it->Point->CtrlPoints(it->Position).size()) {
			// valid point.
			tmplist.push_back(*it);
		}
	}
	list = tmplist;
	if (continuousGridSelection()) {
		CtrlPointSelectionInfo& infof = list.front();
		m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->GCPOffsetInfo().points = infof.Point->CtrlPoints(infof.Position);
		if (infof.Index == 0) {
			*min = 0 - infof.Point->CtrlPoints(infof.Position)[0];
		} else {
			*min = infof.Point->CtrlPoints(infof.Position)[infof.Index - 1]
						 - infof.Point->CtrlPoints(infof.Position)[infof.Index];
		}
		CtrlPointSelectionInfo& infob = list.back();
		if (infob.Index == infob.Point->CtrlPoints(infob.Position).size() - 1) {
			*max = 1 - infob.Point->CtrlPoints(infob.Position)[infob.Index];
		} else {
			*max = infob.Point->CtrlPoints(infob.Position)[infob.Index + 1]
						 - infob.Point->CtrlPoints(infob.Position)[infob.Index];
		}
	}
}

bool GeoDataRiverSurveyCrosssectionWindowGraphicsView::continuousSelection()
{
	QModelIndexList selectedPoints = m_parentWindow->selectionModel()->selectedRows();
	int current = 1;
	while (current < selectedPoints.count()) {
		if ((selectedPoints[current].row() - selectedPoints[current - 1].row()) != 1) {
			return false;
		}
		++current;
	}
	return true;
}

bool GeoDataRiverSurveyCrosssectionWindowGraphicsView::continuousGridSelection()
{
	std::list<CtrlPointSelectionInfo>& list = m_parentWindow->gridCreatingConditionRiverSurvey()->gridCreatingCondition()->selectedCtrlPointInfoList();
	if (list.size() == 0) {return false;}
	else if (list.size() == 1) {return true;}
	else {
		bool ok = true;
		auto it = list.begin();
		CtrlPointSelectionInfo tmpinfo = *it++;
		while (it != list.end()) {
			ok = ok && (tmpinfo.Point == it->Point && tmpinfo.Index + 1 == it->Index);
			tmpinfo = *it++;
		}
		return ok;
	}
}

void GeoDataRiverSurveyCrosssectionWindowGraphicsView::toggleGridCreatingMode(bool gridMode)
{
	m_gridMode = gridMode;
	viewport()->update();
}
