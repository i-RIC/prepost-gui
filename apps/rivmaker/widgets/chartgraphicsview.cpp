#include "chartgraphicsview.h"
#include "chartwindow.h"

#include <QMouseEvent>
#include <QPainter>

#include <cmath>


namespace {

const int ASPECT_RATIO_RIGHT_MARGIN = 10;
const int ASPECT_RATIO_BOTTOM_MARGIN = 10;
const int MIN_SCALE_COUNT = 3;

const int iLeftMargin = 20;
const int iRightMargin = 20;
const int iTopMargin = 20;
const int iBottomMargin = 20;

const double fLeftMargin = 0.1;
const double fRightMargin = 0.1;
const double fTopMargin = 0.1;
const double fBottomMargin = 0.1;

void calcAutoScale(double width, double* scale, double* subScale)
{
	double w = width / MIN_SCALE_COUNT;
	int i = 0;
	while (w > 10) {
		w /= 10;
		++i;
	}
	while (w < 1) {
		w *= 10;
		--i;
	}
	double pow10 = 10;
	if (i < 0) {
		pow10 = 0.1;
		i = -i;
	}
	if (w > 5) {
		*scale = 5 * std::pow(pow10, i);
		*subScale = *scale * 0.2;
	} else if (w > 2) {
		*scale = 2 * std::pow(pow10, i);
		*subScale = *scale * 0.5;
	} else {
		*scale = std::pow(pow10, i);
		*subScale = *scale * 0.2;
	}
}

} // namespace

ChartGraphicsView::ChartGraphicsView(QWidget* w) :
	QWidget {w},
	m_zoomPixmap {":/images/cursorZoom.png"},
	m_movePixmap {":/images/cursorMove.png"},
	m_zoomCursor {m_zoomPixmap, 16, 16},
	m_moveCursor {m_movePixmap, 16, 16},
	m_center {0, 0},
	m_scaleX {1},
	m_scaleY {1},
	m_viewMouseEventMode {vmeNormal}
{
	setMouseTracking(true);
}

void ChartGraphicsView::paintEvent(QPaintEvent* /*event*/)
{
	QPainter painter(this);
	QRect r = rect();
	auto matrix = getMatrix(r);
	drawScales(&painter, matrix);
	drawAspectRatio(&painter);
}

QSize ChartGraphicsView::sizeHint() const
{
	return QSize(640, 480);
}

double ChartGraphicsView::aspectRatio() const
{
	return m_scaleY / m_scaleX;
}

void ChartGraphicsView::setAspectRatio(double ratio)
{
	if (ratio == 0.0) {ratio = 1;}

	double currentRatio = aspectRatio();
	double rate = ratio / currentRatio;
	if (rate > 0.999 && rate < 1.001) {return;}

	zoom(1.0, rate);
}

const ChartGraphicsViewDisplaySetting& ChartGraphicsView::chartDisplaySetting() const
{
	return m_chartDisplaySetting;
}

void ChartGraphicsView::setChartDisplaySetting(const ChartGraphicsViewDisplaySetting& setting)
{
	m_chartDisplaySetting = setting;
}

void ChartGraphicsView::cameraFit()
{
	auto r = getRegion();

	m_center.setX((r.left() + r.right()) * 0.5);
	m_center.setY((r.top() + r.bottom()) * 0.5);


	if (! chartWindow()->fixRegion() && ! chartWindow()->fixAspectRatio()) {
		m_scaleX = (width() - iLeftMargin - iRightMargin) / (r.width() * (1 + fLeftMargin + fRightMargin));
		m_scaleY = (height() - iTopMargin - iBottomMargin) / (r.height() * (1 + fTopMargin + fBottomMargin));
		if (m_scaleX < 0) {m_scaleX = 1;}
		if (m_scaleY < 0) {m_scaleY = 1;}
	}

	update();

	chartWindow()->setAspectRatio(aspectRatio());
}

void ChartGraphicsView::cameraZoomIn()
{
	zoom(1.2, 1.2);
}

void ChartGraphicsView::cameraZoomOut()
{
	zoom(1 / 1.2, 1 / 1.2);
}

void ChartGraphicsView::cameraZoomInX()
{
	zoom(1.2, 1);
}

void ChartGraphicsView::cameraZoomOutX()
{
	zoom(1 / 1.2, 1);
}

void ChartGraphicsView::cameraZoomInY()
{
	zoom(1, 1.2);
}

void ChartGraphicsView::cameraZoomOutY()
{
	zoom(1, 1 / 1.2);
}


void ChartGraphicsView::mouseMoveEventViewChangeHandler(QMouseEvent* event)
{
	int diffx = event->x() - m_oldPosition.x();
	int diffy = event->y() - m_oldPosition.y();

	if (m_viewMouseEventMode == vmeTranslating) {
		translate(diffx, diffy);
	} else if (m_viewMouseEventMode == vmeZooming) {
		double scaleX = 1 + diffx * 0.02;
		double scaleY = 1 - diffy * 0.02;

		if (scaleX < 0.5) {scaleX = 0.5;}
		if (scaleX > 2) {scaleX = 2;}
		if (scaleY < 0.5) {scaleY = 0.5;}
		if (scaleY > 2) {scaleY = 2;}

		if (chartWindow()->fixAspectRatio()) {
			zoom(scaleY, scaleY);
		} else {
			zoom(scaleX, scaleY);
		}
	}
	m_oldPosition = event->pos();
}

void ChartGraphicsView::mousePressEventViewChangeHandler(QMouseEvent* event)
{
	if (event->modifiers() != Qt::ControlModifier) {return;}

	switch (event->button()) {
	case Qt::LeftButton:
		// translate
		if (! chartWindow()->fixRegion()) {
			m_viewMouseEventMode = vmeTranslating;
		}
		break;
	case Qt::MidButton:
		// zoom
		if (! chartWindow()->fixRegion()) {
			m_viewMouseEventMode = vmeZooming;
		}
		break;
	default:
		break;
	}
	m_oldPosition = event->pos();
	updateMouseCursorForViewChange();
}

void ChartGraphicsView::mouseReleaseEventViewChangeHandler(QMouseEvent* /*event*/)
{
	m_viewMouseEventMode = vmeNormal;
	updateMouseCursorForViewChange();
}

void ChartGraphicsView::wheelEventViewChangeHandler(QWheelEvent* event)
{
	if (event->orientation() == Qt::Horizontal) {return;}

	if (chartWindow()->fixRegion()) {return;}

	int numDegrees = event->delta() / 8;
	int numSteps = numDegrees / 15;
	if (numSteps > 0) {
		cameraZoomIn();
	} else {
		cameraZoomOut();
	}
}

void ChartGraphicsView::emitPositionChanged(QMouseEvent* event)
{
	auto matrix = getMatrix(rect()).inverted();
	auto point = matrix.map(QPointF(event->x(), event->y()));

	emit positionChangedForStatusBar(point);
}

void ChartGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
	mouseMoveEventViewChangeHandler(event);
	emitPositionChanged(event);
}

void ChartGraphicsView::mousePressEvent(QMouseEvent* event)
{
	mousePressEventViewChangeHandler(event);
}

void ChartGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	mouseReleaseEventViewChangeHandler(event);
}

void ChartGraphicsView::wheelEvent(QWheelEvent* event)
{
	wheelEventViewChangeHandler(event);
}

void ChartGraphicsView::resizeEvent(QResizeEvent* /*event*/)
{
	chartWindow()->setAspectRatio(aspectRatio());
}

void ChartGraphicsView::paintBackground(QPainter* painter)
{
	painter->fillRect(rect(), QBrush(m_chartDisplaySetting.bgColor));
}

void ChartGraphicsView::drawScales(QPainter* painter, const QMatrix& matrix)
{
	QMatrix invMatrix = matrix.inverted();
	QPointF mins, maxs;
	mins = invMatrix.map(QPointF(0, height()));
	maxs = invMatrix.map(QPointF(width(), 0));

	double xoffset = 5;
	double yoffset = 5;
	double fontoffset = 4;
	double mainruler = 5;
	double subruler = 3;

	double xScale = m_chartDisplaySetting.bgHScaleInterval;
	double xSubScale = m_chartDisplaySetting.bgHSubScaleInterval;
	double yScale = m_chartDisplaySetting.bgVScaleInterval;
	double ySubScale = m_chartDisplaySetting.bgVSubScaleInterval;

	if (m_chartDisplaySetting.bgHScaleAuto) {
		calcAutoScale(maxs.x() - mins.x(), &xScale, &xSubScale);
	}
	if (m_chartDisplaySetting.bgVScaleAuto) {
		calcAutoScale(maxs.y() - mins.y(), &yScale, &ySubScale);
	}

	if (chartWindow()->showGrid()) {
		painter->save();
		if (m_chartDisplaySetting.bgGridType == ChartGraphicsViewDisplaySetting::BackgroundGridType::Lines) {
			// lines
			QPen pen(m_chartDisplaySetting.bgGridColor);
			pen.setStyle(Qt::SolidLine);
			painter->setPen(pen);

			double x = std::floor(mins.x() / xScale) * xScale;
			while (x < maxs.x()) {
				auto from = matrix.map(QPointF(x, maxs.y()));
				auto to = from;
				to.setY(height());
				painter->drawLine(from, to);
				x += xScale;
			}
			double y = std::floor(mins.y() / yScale) * yScale;
			while (y < maxs.y()) {
				auto from = matrix.map(QPointF(mins.x(), y));
				auto to = from;
				to.setX(width());
				painter->drawLine(from, to);
				y += yScale;
			}
			// subscales
			pen.setStyle(Qt::DashLine);
			painter->setPen(pen);
			x = std::floor(mins.x() / xSubScale) * xSubScale;
			while (x < maxs.x()) {
				auto from = matrix.map(QPointF(x, maxs.y()));
				auto to = from;
				to.setY(height());
				painter->drawLine(from, to);
				x += xSubScale;
			}
			y = std::floor(mins.y() / ySubScale) * ySubScale;
			while (y < maxs.y()) {
				auto from = matrix.map(QPointF(mins.x(), y));
				auto to = from;
				to.setX(width());
				painter->drawLine(from, to);
				y += ySubScale;
			}
		} else {
			// dots
			QPen pen(m_chartDisplaySetting.bgGridColor);
			pen.setStyle(Qt::SolidLine);
			painter->setPen(pen);
			QBrush brush(m_chartDisplaySetting.bgGridColor);
			painter->setBrush(brush);

			double x = std::floor(mins.x() / xScale) * xScale;
			while (x < maxs.x()) {
				double y = std::floor(mins.y() / yScale) * yScale;
				while (y < maxs.y()) {
					auto from = matrix.map(QPointF(x, y));
					from.setX(from.x() - 2);
					from.setY(from.y() - 2);
					QPointF to(from.x() + 2, from.y() + 2);
					painter->drawRect(QRectF(from, to));
					y += yScale;
				}
				x += xScale;
			}
			x = std::floor(mins.x() / xSubScale) * xSubScale;
			while (x < maxs.x()) {
				double y = std::floor(mins.y() / ySubScale) * ySubScale;
				while (y < maxs.y()) {
					auto from = matrix.map(QPointF(x, y));
					from.setX(from.x() - 1);
					from.setY(from.y() - 1);
					QPointF to(from.x() + 1, from.y() + 1);
					painter->drawRect(QRectF(from, to));
					y += ySubScale;
				}
				x += xSubScale;
			}
		}
		painter->restore();
	}

	if (chartWindow()->showScales()) {
		painter->save();

		painter->setPen(m_chartDisplaySetting.distanceMarkersColor);
		painter->setFont(m_chartDisplaySetting.distanceMarkersFont);
		QFontMetrics metrics(m_chartDisplaySetting.distanceMarkersFont);

		// Horizontal lines
		QPointF from, to;
		from = QPointF(0, yoffset);
		to = QPointF(width(), yoffset);
		painter->drawLine(from, to);

		// draw X scales
		double x = std::floor(mins.x() / xScale) * xScale;
		while (x < maxs.x()) {
			from = matrix.map(QPointF(x, maxs.y()));
			from.setY(yoffset);
			to = from;
			to.setY(yoffset + mainruler);
			painter->drawLine(from, to);

			QPointF fontPos = to;
			fontPos.setY(yoffset + mainruler + fontoffset);
			QString str = QString("%1").arg(x);
			auto rect = metrics.boundingRect(str);
			QRectF fontRect(fontPos.x() - rect.width() / 2, fontPos.y(), rect.width() + 5, rect.height() + 5);
			painter->drawText(fontRect, Qt::AlignHCenter | Qt::AlignTop, str);
			x += xScale;
		}
		// draw X sub scales.
		x = std::floor(mins.x() / xSubScale) * xSubScale;
		while (x < maxs.x()) {
			from = matrix.map(QPointF(x, maxs.y()));
			from.setY(from.y() + yoffset);
			to = from;
			to.setY(to.y() + yoffset + subruler);
			painter->drawLine(from, to);
			x += xSubScale;
		}

		// Vertical lines
		from = QPointF(xoffset, 0);
		to = QPointF(xoffset, height());
		painter->drawLine(from, to);

		// draw Y scales
		double y = std::floor(mins.y() / yScale) * yScale;
		while (y < maxs.y()) {
			from = matrix.map(QPointF(mins.x(), y));
			from.setX(xoffset);
			to.setX(xoffset + mainruler);
			to.setY(from.y());
			painter->drawLine(from, to);

			QPointF fontPos = to;
			fontPos.setX(xoffset + mainruler + fontoffset);
			QString str = QString("%1").arg(y);
			auto rect = metrics.boundingRect(str);
			QRectF fontRect(fontPos.x(), fontPos.y() - rect.height() / 2, rect.width() + 5, rect.height() + 5);
			painter->drawText(fontRect, Qt::AlignLeft | Qt::AlignVCenter, str);
			y += yScale;
		}
		// draw Y sub scales.
		y = std::floor(mins.y() / ySubScale) * ySubScale;
		while (y < maxs.y()) {
			from = matrix.map(QPointF(mins.x(), y));
			from.setX(xoffset);
			to.setX(xoffset + subruler);
			to.setY(from.y());
			painter->drawLine(from, to);
			y += ySubScale;
		}
		painter->restore();
	}
}

void ChartGraphicsView::drawAspectRatio(QPainter* painter)
{
	if (! chartWindow()->showAspectRatio()) {return;}

	painter->save();
	painter->setPen(m_chartDisplaySetting.aspectRatioColor);
	painter->setFont(m_chartDisplaySetting.aspectRatioFont);

	QSize s = size();

	auto str = tr("Aspect ratio: 1 / %1").arg(aspectRatio());
	QFontMetricsF metrics(painter->font());
	auto rect = metrics.boundingRect(str);

	auto left = s.width() - ASPECT_RATIO_RIGHT_MARGIN - rect.width();
	auto top = s.height() - ASPECT_RATIO_BOTTOM_MARGIN - rect.height();
	QRectF fontRect(left, top, rect.width() + 1, rect.height() + 1);
	painter->drawText(fontRect, Qt::AlignLeft | Qt::AlignTop, str);

	painter->restore();
}

ChartWindow* ChartGraphicsView::chartWindow() const
{
	return dynamic_cast<ChartWindow*> (parentWidget());
}

void ChartGraphicsView::zoom(double scaleX, double scaleY)
{
	m_scaleX *= scaleX;
	m_scaleY *= scaleY;

	update();

	chartWindow()->setAspectRatio(aspectRatio());
}

void ChartGraphicsView::translate(int x, int y)
{
	m_center.setX(m_center.x() - x / m_scaleX);
	m_center.setY(m_center.y() + y / m_scaleY);

	update();

	chartWindow()->setAspectRatio(aspectRatio());
}

void ChartGraphicsView::updateMouseCursorForViewChange()
{
	if (m_viewMouseEventMode == vmeZooming) {
		setCursor(m_zoomCursor);
	} else if (m_viewMouseEventMode == vmeTranslating) {
		setCursor(m_moveCursor);
	} else {
		setCursor(Qt::ArrowCursor);
	}
}

QRectF ChartGraphicsView::getRegion()
{
	return QRectF(0, 0, 1, 1);
}

QMatrix ChartGraphicsView::getMatrix(const QRect& viewport)
{
	QMatrix translate1, scale, translate2;

	translate1 = QMatrix(1, 0, 0, 1, - m_center.x(), - m_center.y());
	scale = QMatrix(m_scaleX, 0, 0, - m_scaleY, 0, 0);
	translate2 = QMatrix(1, 0, 0, 1, viewport.width() * 0.5, viewport.height() * 0.5);

	return translate1 * scale * translate2;
}

ChartGraphicsView::ViewMouseEventMode ChartGraphicsView::viewMouseEventMode() const
{
	return m_viewMouseEventMode;
}
