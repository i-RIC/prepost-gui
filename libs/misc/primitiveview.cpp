#include "primitiveview.h"

#include "ui_primitiveview.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

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

QTransform getTransform(const QSize& s, const QPointF& center, double scaleX, double scaleY, double angleDegree, double reverseYCoeff)
{
	QTransform t;
	t.translate(s.width() / 2., s.height() / 2.);
	t.rotate(- angleDegree);
	t.scale(scaleX, scaleY * reverseYCoeff);
	t.translate(-center.x(), center.y() * reverseYCoeff);

	return t;
}
} // namespace

PrimitiveView::PrimitiveView(QWidget *parent) :
	PrimitiveView(parent, false)
{}

PrimitiveView::PrimitiveView(QWidget *parent, bool reverseY) :
	QWidget {parent},
	m_transform {},
	m_centerPoint {},
	m_scale {1},
	m_relativeScaleX {1},
	m_relativeScaleY {1},
	m_angleDegree {0.},
	m_reverseYCoeff {reverseY ? 1. : -1.},
	m_zooming {false},
	m_translating {false},
	m_rotating {false},
	m_previousPos {},
	m_drawOffset {},
	m_rightClickPos {},
	m_zoomCursor {QPixmap {":/libs/guibase/images/cursorZoom.png"}, 16, 16},
	m_moveCursor {QPixmap {":/libs/guibase/images/cursorMove.png"}, 16, 16},
	m_rotateCursor {QPixmap {":/libs/guibase/images/cursorRotate.png"}, 16, 16},
	ui {new Ui::PrimitiveView}
{
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	ui->setupUi(this);
}

PrimitiveView::~PrimitiveView()
{
	delete ui;
}

void PrimitiveView::updateView()
{
	updateTransform();
	update();
}

void PrimitiveView::mouseMoveEvent(QMouseEvent* event)
{
	if (inViewOperation()) {
		viewMouseMoveEvent(event);

		updateTransform();
		update();
		return;
	}
}

void PrimitiveView::mousePressEvent(QMouseEvent* event)
{
	viewMousePressEvent(event);
	if (inViewOperation()) {return;}

	if (event->button() == Qt::RightButton) {
		m_rightClickPos = event->pos();
	}
}

void PrimitiveView::mouseReleaseEvent(QMouseEvent* event)
{
	if (inViewOperation()) {
		viewMouseReleaseEvent(event);
		return;
	}
}

void PrimitiveView::wheelEvent(QWheelEvent* event)
{
	viewWheelEvent(event);
}

void PrimitiveView::viewMouseMoveEvent(QMouseEvent* event)
{
	if (m_zooming) {
		int deltaY = event->pos().y() - m_previousPos.y();
		m_scale *= std::exp(- deltaY * 0.01);
	} else if (m_translating) {
		double transX = (event->pos().x() - m_previousPos.x());
		double transY = - (event->pos().y() - m_previousPos.y());
		QVector2D delta(transX / m_relativeScaleX, transY/ m_relativeScaleY);
		delta /= m_scale;
		rotate(&delta, m_angleDegree * m_reverseYCoeff);

		m_centerPoint.setX(m_centerPoint.x() - delta.x());
		m_centerPoint.setY(m_centerPoint.y() - delta.y());
	} else if (m_rotating) {
		QSize s = size();
		QPointF center(s.width() * 0.5, s.height() * 0.5);
		QVector2D v1(m_previousPos.x() - center.x(), - (m_previousPos.y() - center.y()));
		QVector2D v2(event->x() - center.x(), - (event->y() - center.y()));
		double a = ::angleDegree(v1, v2);
		m_angleDegree = m_angleDegree + a;
		double r = static_cast<int>(m_angleDegree / 360);
		m_angleDegree -= r * 360;
	} else {
		return;
	}
	m_previousPos = event->pos();
}

void PrimitiveView::viewMousePressEvent(QMouseEvent* event)
{
	m_previousPos = event->pos();
	if (event->modifiers() == Qt::ControlModifier) {
		if (event->button() == Qt::LeftButton) {
			m_translating = true;
			setCursor(m_moveCursor);
		} else if (event->button() == Qt::MidButton) {
			m_zooming = true;
			setCursor(m_zoomCursor);
		// } else if (event->button() == Qt::RightButton) {
		// 	m_rotating = true;
		// 	setCursor(m_rotateCursor);
		}
	}
}

void PrimitiveView::viewMouseReleaseEvent(QMouseEvent*)
{
	m_zooming = false;
	m_translating = false;
	m_rotating = false;
	setCursor(Qt::ArrowCursor);

	// auto c = model()->selectedItemController();
	// if (c != nullptr){
	// 	c->restoreMouseCursorOnView(this);
	// }
	updateView();
}

void PrimitiveView::viewWheelEvent(QWheelEvent* event)
{
	int delta = event->delta();
	if (delta < 0) {
		m_scale /= 2;
	} else if (delta > 0) {
		m_scale *= 2;
	}
	updateView();
}

void PrimitiveView::paintEvent(QPaintEvent*)
{
	if (! inViewOperation()) {
		m_cachedInvertedTransform = m_transform.inverted();
		m_cachePixmap = drawModel();
	}

	paintEvent(this, m_cachePixmap, Qt::darkGray, m_cachedInvertedTransform * m_transform);
}

void PrimitiveView::paintEvent(QWidget* widget, const QPixmap& pixmap, const QColor& backgroundColor, const QTransform& transform) const
{
	QPainter painter(widget);
	painter.fillRect(widget->rect(), backgroundColor);

	painter.save();
	painter.setTransform(transform);
	painter.drawPixmap(0, 0, pixmap);
	painter.restore();
}

void PrimitiveView::setCenterPoint(const QPointF& centerPoint)
{
	m_centerPoint = centerPoint;
}

double PrimitiveView::scale()
{
	return m_scale;
}

void PrimitiveView::setScale(double scale)
{
	m_scale = scale;
}

void PrimitiveView::setRelativeScaleX(double scale)
{
	m_relativeScaleX = scale;
}

void PrimitiveView::setRelativeScaleY(double scale)
{
	m_relativeScaleY = scale;
}

double PrimitiveView::angleDegree() const
{
	return m_angleDegree;
}

void PrimitiveView::setAngleDegree(double)
{
	// m_angleDegree = angle; // Rotation disabled.
}

const QTransform& PrimitiveView::transform()
{
	return m_transform;
}

const QPoint& PrimitiveView::rightClickPos()
{
	return m_rightClickPos;
}

void PrimitiveView::updateTransform()
{
	QPointF center = m_centerPoint - m_drawOffset;
	m_transform = getTransform(size(), center, m_scale * m_relativeScaleX, m_scale * m_relativeScaleY, m_angleDegree, m_reverseYCoeff);

	emit drawnAreaChanged();
}

QPixmap PrimitiveView::drawModel()
{
	QPixmap pixmap(size());

	QPainter painter(&pixmap);
	painter.fillRect(rect(), Qt::darkGray);

	QRectF r = drawnArea();

	return pixmap;
}

bool PrimitiveView::inViewOperation() const
{
	return m_translating || m_rotating || m_zooming;
}

QPointF PrimitiveView::conv(const QPointF& p) const
{
	return m_transform.map(p);
}

// data coord -> widget coord
QPointF PrimitiveView::rconv(const QPointF& p) const
{
	return m_transform.inverted().map(p);
}

QRectF PrimitiveView::drawnArea() const
{
	return drawnArea(*this, m_transform, m_drawOffset);
}

QRectF PrimitiveView::drawnArea(const QWidget& widget, const QTransform& transform, const QPointF& drawOffset) const
{
	QRectF r = widget.rect();
	QTransform t = transform.inverted();
	QRectF r2 = t.mapRect(r);
	QRectF r3;
	r3.setTopLeft(QPointF(r2.left(), -r2.bottom()));
	r3.setBottomRight(QPointF(r2.right(), -r2.top()));
	r3.translate(drawOffset);

	return r3;
}
