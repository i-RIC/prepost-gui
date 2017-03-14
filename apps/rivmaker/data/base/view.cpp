#include "model.h"
#include "view.h"
#include "dataitemcontroller.h"

#include <QPainter>

View::View(QWidget* parent) :
	QWidget {parent}
{
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	setFocus();

	setupCursors();
}

View::~View()
{}

void View::updateView()
{
	// @todo implement this
}

QPointF View::conv(const QPointF& point) const
{
	QPointF ret = point;
	ret -= m_drawOffset;
	ret.setY(- ret.y());

	return m_transform.map(ret);
}

QPointF View::rconv(const QPointF& point) const
{
	QPointF ret = m_transform.inverted().map(point);
	ret.setY(- ret.y());
	ret += m_drawOffset;
	return ret;
}

QRectF View::drawnRegion() const
{
	QSize s = size();
	QTransform inv = m_transform.inverted();

	QRectF srcrect(0, 0, s.width(), s.height());
	QRectF mappedRect = inv.mapRect(srcrect);
	QPointF offset = m_drawOffset;

	return QRectF(mappedRect.left() + offset.x(), - mappedRect.bottom() + offset.y(), mappedRect.width(), mappedRect.height());
}

const QTransform& View::transform() const
{
	return m_transform;
}

Model* View::model() const
{
	return m_model;
}

void View::setModel(Model* m)
{
	m_model = m;
}

QPointF View::drawnPos(const QPoint& pos) const
{
	QPointF c = m_drawOffset;
	QPointF p1(pos);
	QTransform t = m_transform.inverted();
	QPointF p2 = t.map(p1);
	p2.setX(p2.x() + c.x());
	p2.setY(-p2.y() + c.y());
	return p2;
}

bool View::isClick(const QPoint& pressP, const QPoint& releaseP)
{
	if (qAbs(pressP.x() - releaseP.x()) > CLICK_LIMIT) {return false;}
	if (qAbs(pressP.y() - releaseP.y()) > CLICK_LIMIT) {return false;}

	return true;
}

void View::fit()
{
	paramsFit();
	updateView();
}

void View::zoomIn()
{
	paramsZoomIn();
	updateView();
}

void View::zoomOut()
{
	paramsZoomOut();
	updateView();
}

void View::resetRotation()
{
	paramsResetRotation();
	updateView();
}

void View::rotate90()
{
	paramsRotate90();
	updateView();
}

void View::paintEvent(QPaintEvent* event)
{
}

void View::keyPressEvent(QKeyEvent* event)
{
	auto c = m_model->selectedItemController();
	if (c == nullptr) {return;}

	c->keyPressEvent(event, this);
}

void View::keyReleaseEvent(QKeyEvent* event)
{
	auto c = m_model->selectedItemController();
	if (c == nullptr) {return;}

	c->keyReleaseEvent(event, this);
}

void View::mouseDoubleClickEvent(QMouseEvent* event)
{
	auto c = m_model->selectedItemController();
	if (c == nullptr) {return;}

	c->mouseDoubleClickEvent(event, this);
}

void View::mouseMoveEvent(QMouseEvent* event)
{
	auto c = m_model->selectedItemController();
	if (c == nullptr) {return;}

	c->mouseMoveEvent(event, this);
}

void View::mousePressEvent(QMouseEvent* event)
{

}

void View::mouseReleaseEvent(QMouseEvent* event)
{

}

void View::wheelEvent(QWheelEvent* event)
{

}

void View::resizeEvent(QResizeEvent* event)
{

}

void View::paramsResetRotation()
{}

void View::paramsRotate90()
{}

void View::emitPosition(QMouseEvent* event)
{

}

void View::setupCursors()
{

}

bool View::prepareDrawModel()
{
	return true;
}
