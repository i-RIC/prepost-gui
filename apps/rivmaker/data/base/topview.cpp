#include "dataitemcontroller.h"
#include "dataitemview.h"
#include "model.h"
#include "topview.h"

#include <QMouseEvent>

#define _USE_MATH_DEFINES
#include <math.h>

namespace {

void rotate(QVector2D* vec, double angle_degree)
{
	double angle_rad = angle_degree / 180. * M_PI;

	double x = vec->x();
	double y = vec->y();

	x = + std::cos(angle_rad) * x + std::sin(angle_rad) * y;
	y = - std::sin(angle_rad) * x + std::cos(angle_rad) * y;

	vec->setX(x);
	vec->setY(y);
}

} // namespace

TopView::TopView(QWidget* parent) :
	View {parent}
{}

void TopView::paramsFit()
{
	double centerX = 0, centerY = 0;
	double width = 1, height = 1;

	QRectF bb = model()->rootDataItemView()->boundingBox();
	if (! bb.isNull()) {
		auto c = bb.center();
		centerX = c.x();
		centerY = c.y();
		width = bb.width();
		height = bb.height();
	}
	QSize s = size();
	double xScale = s.width() / width;
	double yScale = s.height() / height;

	m_scale = qMin(xScale, yScale);
	m_drawOffset = bb.center();
	m_centerPoint = bb.center();
}

void TopView::paramsZoomIn()
{
	m_scale /= 2;
}

void TopView::paramsZoomOut()
{
	m_scale *= 2;
}

void TopView::paramsResetRotation()
{
	m_angleDegree = 0;
}

void TopView::paramsRotate90()
{
	m_angleDegree += 90;
}

void TopView::viewMouseMoveEvent(QMouseEvent* event)
{
	if (m_zooming) {
		int deltaY = event->pos().y() - m_previousPos.y();
		m_scale *= std::exp(- deltaY * 0.01);
	} else if (m_translating) {
		double transX = (event->pos().x() - m_previousPos.x());
		double transY = - (event->pos().y() - m_previousPos.y());
		QVector2D delta(transX, transY);
		delta /= m_scale;
		rotate(&delta, m_angleDegree);

		m_centerPoint.setX(m_centerPoint.x() - delta.x());
		m_centerPoint.setY(m_centerPoint.y() - delta.y());
	} else {
		return;
	}
	m_previousPos = event->pos();
}

void TopView::viewMousePressEvent(QMouseEvent* event)
{
	m_previousPos = event->pos();
	if (event->modifiers() == Qt::ControlModifier) {
		if (event->button() == Qt::LeftButton) {
			m_translating = true;
			setCursor(m_moveCursor);
		} else if (event->button() == Qt::MidButton) {
			m_zooming = true;
			setCursor(m_zoomCursor);
		}
	}
}

void TopView::viewMouseReleaseEvent(QMouseEvent*)
{
	m_zooming = false;
	m_translating = false;
	setCursor(Qt::ArrowCursor);

	auto c = model()->selectedItemController();
	if (c != nullptr){
		c->restoreMouseCursorOnView(this);
	}
	updateView();
}

void TopView::viewWheelEvent(QWheelEvent* event)
{
	int delta = event->delta();
	if (delta < 0) {
		m_scale /= 2;
	} else if (delta > 0) {
		m_scale *= 2;
	}
	updateView();
}

void TopView::updateTransform()
{
	QSize s = size();

	QTransform t;
	QPointF center = m_centerPoint - m_drawOffset;
	t.translate(center.x(), -center.y());
	t.scale(1 / m_scale, 1 / m_scale);
	t.rotate(m_angleDegree);
	t.translate(- s.width() / 2., - s.height() / 2.);

	m_transform = t.inverted();
}
