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

	double newX = + std::cos(angle_rad) * x + std::sin(angle_rad) * y;
	double newY = - std::sin(angle_rad) * x + std::cos(angle_rad) * y;

	vec->setX(newX);
	vec->setY(newY);
}

void rotateVector90(QVector2D& v)
{
	double tmp = v.x();
	v.setX(- v.y());
	v.setY(tmp);
}

double angleRadian(const QVector2D& v1, const QVector2D& v2)
{
	qreal dotprod = QVector2D::dotProduct(v1, v2);
	double cosVal = dotprod / (v1.length() * v2.length());
	if (cosVal <= -1) {return M_PI;}
	if (cosVal >= 1) {return 0;}
	qreal angle1 = acos(cosVal);
	QVector2D tmp = v1;
	rotateVector90(tmp);
	if (QVector2D::dotProduct(tmp, v2) < 0) {
		return 2 * M_PI - angle1;
	} else {
		return angle1;
	}
}

double angleDegree(const QVector2D& v1, const QVector2D& v2)
{
	return angleRadian(v1, v2) / M_PI * 180.;
}

} // namespace

TopView::TopView(QWidget* parent) :
	View {parent}
{}

void TopView::paramsFit()
{
	double centerX = 0, centerY = 0;
	double width = 1, height = 1;

	QRectF bb;
	auto view = model()->rootDataItemView();

	if (view != nullptr) {
		bb = view->boundingBox();
	}

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
	m_scale *= 2;
}

void TopView::paramsZoomOut()
{
	m_scale /= 2;
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
	} else if (m_rotating) {
		QSize s = size();
		QPointF center(s.width() * 0.5, s.height() * 0.5);
		QVector2D v1(m_previousPos.x() - center.x(), - (m_previousPos.y() - center.y()));
		QVector2D v2(event->x() - center.x(), - (event->y() - center.y()));
		double a = angleDegree(v1, v2);
		m_angleDegree = m_angleDegree + a;
		double r = static_cast<int>(m_angleDegree / 360);
		m_angleDegree -= r * 360;
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
		} else if (event->button() == Qt::RightButton) {
			m_rotating = true;
			setCursor(m_rotateCursor);
		}
	}
}

void TopView::viewMouseReleaseEvent(QMouseEvent*)
{
	m_zooming = false;
	m_translating = false;
	m_rotating = false;
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

bool TopView::inViewOperation()
{
	return m_translating || m_rotating || m_zooming;
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
