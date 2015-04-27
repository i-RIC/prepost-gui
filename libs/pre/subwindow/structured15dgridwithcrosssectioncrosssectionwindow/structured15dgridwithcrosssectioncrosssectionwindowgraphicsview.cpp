#include <cmath>

#include "structured15dgridwithcrosssectioncrosssectionwindowgraphicsview.h"
#include "structured15dgridwithcrosssectioncrosssectionwindow.h"
#include "structured15dgridwithcrosssectioncrosssectionaltitudemovedialog.h"

#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QMouseEvent>

Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView(QWidget* w)
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
	setupActions();
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::setupActions()
{
	m_moveAction = new QAction(tr("&Move"), this);
	connect(m_moveAction, SIGNAL(triggered()), this, SLOT(moveSelectedRows()));
	m_moveAction->setEnabled(false);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::setupMenu()
{
	if (m_rightClickingMenu == nullptr){
		m_rightClickingMenu = new QMenu(this);
		m_rightClickingMenu->addAction(m_moveAction);
		m_rightClickingMenu->addAction(m_parentWindow->deleteAction());
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::dataChanged(const QModelIndex& /*topLeft*/, const QModelIndex& /*bottomRight*/)
{
	viewport()->update();
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::paintEvent(QPaintEvent* /*event*/)
{
	QPainter painter(viewport());
	QRect vp = painter.viewport();
	QMatrix matrix = getMatrix(vp);
	m_matrix = matrix;
	painter.setRenderHint(QPainter::Antialiasing);
	// Draw scales.
	drawScales(painter, matrix);

	// draw lines.
	QPen pen = QPen(Qt::black, 1);
	painter.setPen(pen);
	drawLine(m_parentWindow->m_blackLineCrossSection, painter);

	// draw circles.
	drawCircle(painter);
	// draw selected circles.
	drawSelectionCircle(painter);
}

QMatrix Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::getMatrix(QRect& viewport)
{
	QRectF region = m_drawnRegion;
	QMatrix translate1, scale, translate2;
	double xlength = region.right() - region.left();
	double ylength = region.bottom() - region.top();
	if (xlength == 0) xlength = 1;
	if (ylength == 0) ylength = 1;

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

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::drawLine(Structured15DGridWithCrossSectionCrossSection *section, QPainter &painter)
{
	if (section == nullptr) return;
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& alist = section->altitudeInfo();
	bool first = true;
	QPointF oldpoint, newpoint;
	for (auto it = alist.begin(); it != alist.end(); ++it){
		Structured15DGridWithCrossSectionCrossSection::Altitude alt = *it;
		newpoint = m_matrix.map(QPointF(alt.m_position, alt.m_height));
		if (! first){
			painter.drawLine(oldpoint, newpoint);
		}
		oldpoint = newpoint;
		first = false;
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::drawCircle(QPainter& painter)
{
	QPen pen(Qt::black, 1);
	QBrush activeBrush(Qt::red, Qt::SolidPattern);

	Structured15DGridWithCrossSectionCrossSection* cross = m_parentWindow->m_blackLineCrossSection;
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& alist = cross->altitudeInfo();
	painter.setPen(pen);
	for (auto it = alist.begin(); it != alist.end(); ++it){
		Structured15DGridWithCrossSectionCrossSection::Altitude alt = *it;
		painter.setBrush(activeBrush);
		QPointF point = m_matrix.map(QPointF(alt.m_position, alt.m_height));
		QRect r(point.x() - ellipseR, point.y() - ellipseR, ellipseR * 2, ellipseR * 2);
		painter.drawEllipse(r);
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::drawSelectionCircle(QPainter& painter)
{
	QPen pen(Qt::black, 1);
	QBrush activeBrush(Qt::red, Qt::SolidPattern);

	Structured15DGridWithCrossSectionCrossSection* cross = m_parentWindow->m_blackLineCrossSection;
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& alist = cross->altitudeInfo();
	painter.setPen(pen);
	QModelIndexList list = selectionModel()->selectedRows();
	for (auto it = list.begin(); it != list.end(); ++it){
		QModelIndex index = *it;
		const Structured15DGridWithCrossSectionCrossSection::Altitude& alt = alist.at(index.row());
		painter.setBrush(activeBrush);
		QPointF point = m_matrix.map(QPointF(alt.m_position, alt.m_height));
		QRectF r(point.x() - selectedEllipseR, point.y() - selectedEllipseR, selectedEllipseR * 2, selectedEllipseR * 2);
		painter.drawEllipse(r);
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::drawScales(QPainter& painter, const QMatrix& matrix)
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
	while (xdwidth > 10){
		xdwidth /= 10.;
		++i;
	}
	while (xdwidth < 1){
		xdwidth *= 10.;
		--i;
	}
	// now 1 < xdwidth < 10.
	double dx;
	double pow10 = 10;
	if (i < 0){
		pow10 = 0.1;
		i = - i;
	}
	if (xdwidth > 5){
		xdwidth = 5 * std::pow(pow10, i);
		dx = 0.2;
	}else if (xdwidth > 2){
		xdwidth = 2 * std::pow(pow10, i);
		dx = 0.5;
	}else{
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
	while (x < maxs.x()){
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
	while (x < maxs.x()){
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
	while (ydwidth > 10){
		ydwidth /= 10.;
		++i;
	}
	while (ydwidth < 1){
		ydwidth *= 10.;
		--i;
	}
	// now 1 < ydwidth < 10.
	double dy;
	pow10 = 10;
	if (i < 0){
		pow10 = 0.1;
		i = - i;
	}
	if (ydwidth > 5){
		ydwidth = 5 * std::pow(pow10, i);
		dy = 0.2;
	}else if (ydwidth > 2){
		ydwidth = 2 * std::pow(pow10, i);
		dy = 0.5;
	}else{
		ydwidth = std::pow(pow10, i);
		dy = 1.0;
	}
	rulemin = std::floor(mins.y() / ydwidth) * ydwidth;

	// draw Y main scales.
	from = QPointF(xoffset, 0);
	to = QPointF(xoffset, w->height());
	painter.drawLine(from, to);

	double y = rulemin;
	while (y < maxs.y()){
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
	while (y < maxs.y()){
		y += ydwidth * dy;
		from = matrix.map(QPointF(mins.x(), y));
		from.setX(xoffset);
		to.setX(xoffset + subruler);
		to.setY(from.y());
		painter.drawLine(from, to);
	}
	/*
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
	 */
	painter.setPen(oldPen);
}

QRectF Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::getRegion()
{
	QRectF ret(0., 0., 0., 0.);
	QAbstractItemModel* m = model();
	for (int i = 0; i < m->rowCount(); ++i){
		double x = m->data(m->index(i, 0)).toDouble();
		double y = m->data(m->index(i, 1)).toDouble();
		if (i == 0 || x < ret.left()) ret.setLeft(x);
		if (i == 0 || x > ret.right()) ret.setRight(x);
		if (i == 0 || y < ret.top()) ret.setTop(y);
		if (i == 0 || y > ret.bottom()) ret.setBottom(y);
	}
	return ret;
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraFit()
{
	QRect vp = viewport()->rect();
	m_drawnRegion = getRegion();
//	m_matrix = getMatrix(vp);
	viewport()->update();
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraMoveLeft()
{
	translate(- moveWidth(), 0);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraMoveRight()
{
	translate(moveWidth(), 0);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraMoveUp()
{
	translate(0, - moveWidth());
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraMoveDown()
{
	translate(0, moveWidth());
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraZoomIn()
{
	zoom(1.2, 1.2);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraZoomOut()
{
	zoom(1. / 1.2, 1. / 1.2);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraZoomInX()
{
	zoom(1.2, 1);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraZoomOutX()
{
	zoom(1. / 1.2, 1);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraZoomInY()
{
	zoom(1, 1.2);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::cameraZoomOutY()
{
	zoom(1, 1. / 1.2);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	int diffx = event->x() - m_oldPosition.x();
	int diffy = event->y() - m_oldPosition.y();
	switch (m_mouseEventMode){
	case meNormal:
		// do nothing.
		break;
	case meTranslating:
		translate(diffx, diffy);
		break;
	case meZooming:
	{
		double scaleX = 1 + diffx * 0.02;
		double scaleY = 1 - diffy * 0.02;
		if (scaleX < 0.5) scaleX = 0.5;
		if (scaleY < 0.5) scaleY = 0.5;
		if (scaleX > 2) scaleX = 2;
		if (scaleY > 2) scaleY = 2;
		zoom(scaleX, scaleY);
		break;
	}
	case meSelecting:
	{
		QPoint topLeft(qMin(m_rubberOrigin.x(), event->x()), qMin(m_rubberOrigin.y(), event->y()));
		QSize size(qAbs(m_rubberOrigin.x() - event->x()), qAbs(m_rubberOrigin.y() - event->y()));
		QRect rect(topLeft, size);
		m_rubberBand->setGeometry(rect);
		viewport()->update();
		break;
	}
	}
	m_oldPosition = event->pos();
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::mousePressEvent(QMouseEvent* event)
{
	switch (m_mouseEventMode){
	case meNormal:
		if (event->modifiers() == Qt::ControlModifier){
			switch (event->button()){
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
			}
			m_oldPosition = event->pos();
			updateMouseCursor();
		} else {
			if (event->button() == Qt::LeftButton){
				// start selecting.
				m_mouseEventMode = meSelecting;
				if (m_rubberBand == nullptr){
					m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
				}
				m_rubberOrigin = event->pos();
				m_rubberBand->setGeometry(m_rubberOrigin.x(), m_rubberOrigin.y(), 0, 0);
				m_rubberBand->show();
			} else if (event->button() == Qt::RightButton){
				m_dragStartPoint = event->pos();
			}
		}
		break;
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	switch (m_mouseEventMode){
	case meNormal:
		if (event->button() == Qt::RightButton){
			if (ProjectDataItem::isNear(m_dragStartPoint, event->pos())){
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
		// select the point inside the rubberband geometry.
		selectPoints(m_rubberOrigin, event->pos());

		m_mouseEventMode = meNormal;
		// go back to normal mode.
		updateMouseCursor();
		break;
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::wheelEvent(QWheelEvent *event)
{
	if (event->orientation() == Qt::Horizontal){return;}
	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	if (numSteps > 0){
		cameraZoomIn();
	}else{
		cameraZoomOut();
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::zoom(double scaleX, double scaleY)
{
	qreal drawnRegionCenterX = (m_drawnRegion.left() + m_drawnRegion.right()) * .5;
	qreal drawnRegionCenterY = (m_drawnRegion.top() + m_drawnRegion.bottom()) * .5;
	qreal xWidth = m_drawnRegion.right() - m_drawnRegion.left();
	qreal yWidth = m_drawnRegion.top() - m_drawnRegion.bottom();

	qreal newxWidth = xWidth / scaleX;
	qreal newyWidth = yWidth / scaleY;

	m_drawnRegion.setLeft(drawnRegionCenterX - newxWidth * .5);
	m_drawnRegion.setRight(drawnRegionCenterX + newxWidth * .5);
	m_drawnRegion.setTop(drawnRegionCenterY + newyWidth * .5);
	m_drawnRegion.setBottom(drawnRegionCenterY - newyWidth * .5);
	viewport()->update();
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::translate(int x, int y)
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

int Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::moveWidth()
{
	QWidget* w = viewport();
	int stdW = qMax(w->width(), w->height());
	return stdW / 3;
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::updateMouseCursor()
{
	switch (m_mouseEventMode){
	case meNormal:
		setCursor(Qt::ArrowCursor);
		break;
	case meZooming:
		setCursor(m_zoomCursor);
		break;
	case meTranslating:
		setCursor(m_moveCursor);
		break;
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::selectPoints(const QPoint& from, const QPoint& to)
{
	QPointF mins(qMin(from.x(), to.x()), qMax(from.y(), to.y()));
	QPointF maxs(qMax(from.x(), to.x()), qMin(from.y(), to.y()));

	QMatrix invMatrix = m_matrix.inverted();
	QPointF mappedMins = invMatrix.map(mins);
	QPointF mappedMaxs = invMatrix.map(maxs);

	QItemSelection selection;

	Structured15DGridWithCrossSectionCrossSection* cross = m_parentWindow->m_blackLineCrossSection;
	QVector<Structured15DGridWithCrossSectionCrossSection::Altitude>& alist = cross->altitudeInfo();
	int row = 0;
	for (auto it = alist.begin(); it != alist.end(); ++it){
		Structured15DGridWithCrossSectionCrossSection::Altitude alt = *it;
		if (alt.m_position >= mappedMins.x() &&
		    alt.m_position <= mappedMaxs.x() &&
		    alt.m_height >= mappedMins.y() &&
		    alt.m_height <= mappedMaxs.y())
		{
			selection.merge(QItemSelection(model()->index(row, 0), model()->index(row, 1)), QItemSelectionModel::Select);
		}
		++row;
	}
	selectionModel()->clearSelection();
	selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::updateActionStatus()
{
	QModelIndexList rows = selectionModel()->selectedRows();
	if (rows.count() == 0){
		m_moveAction->setDisabled(true);
	} else {
		bool continuous = true;
		auto it = rows.begin();
		auto it2 = it;
		++it2;
		while (it2 != rows.end()){
			continuous = continuous && (it2->row() == it->row() + 1);
			++it, ++it2;
		}
		m_moveAction->setEnabled(continuous);
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowGraphicsView::moveSelectedRows()
{
	QModelIndexList rows = selectionModel()->selectedRows();
	int from = rows.front().row();
	int to = rows.back().row();
	Structured15DGridWithCrossSectionCrossSectionAltitudeMoveDialog dialog(m_parentWindow->m_blackLineCrossSection, from, to, m_parentWindow, m_parentWindow->m_shapeItem, this);
	dialog.exec();
}
