#include "model.h"
#include "view.h"
#include "dataitemcontroller.h"

#include <QColor>
#include <QMouseEvent>
#include <QPainter>

const int View::CLICK_LIMIT = 3;
const int View::NEAR_LIMIT = 3;

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
	updateTransform();
	if (! model()->rootDataItemView()->prepareDraw()) {return;}

	update();
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

bool View::isNear(const QPointF& p1, const QPointF& p2)
{
	if (qAbs(p1.x() - p2.x()) > NEAR_LIMIT) {return false;}
	if (qAbs(p1.y() - p2.y()) > NEAR_LIMIT) {return false;}

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
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(rect(), QColor(250, 250, 250));

	model()->rootDataItemView()->draw(&painter);
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
	if (inViewOperation()) {
		viewMouseMoveEvent(event);
		updateTransform();
		update();
		return;
	}
	auto c = m_model->selectedItemController();
	if (c == nullptr) {return;}

	c->mouseMoveEvent(event, this);
}

void View::mousePressEvent(QMouseEvent* event)
{
	if (event->modifiers() == Qt::ControlModifier) {
		viewMousePressEvent(event);
		return;
	}

	auto c = m_model->selectedItemController();
	if (c == nullptr) {return;}

	c->mousePressEvent(event, this);

	if (event->button() == Qt::RightButton) {
		m_rightClickPos = event->pos();
	}
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
	if (inViewOperation()) {
		viewMouseReleaseEvent(event);
		return;
	}

	auto c = m_model->selectedItemController();
	if (c == nullptr) {return;}

	c->mouseReleaseEvent(event, this);

	if (event->button() != Qt::RightButton) {return;}
	if (! isClick(m_rightClickPos, event->pos())) {return;}
	m_model->showRightClickMenu(event->globalPos());
}

void View::wheelEvent(QWheelEvent* event)
{
	viewWheelEvent(event);
}

void View::resizeEvent(QResizeEvent*)
{}

void View::paramsResetRotation()
{}

void View::paramsRotate90()
{}

void View::emitPosition(QMouseEvent* event)
{

}

void View::setupCursors()
{
	m_zoomPixmap = QPixmap(":/images/cursorZoom.png");
	m_movePixmap = QPixmap(":/images/cursorMove.png");

	m_zoomCursor = QCursor(m_zoomPixmap);
	m_moveCursor = QCursor(m_movePixmap);
}

bool View::prepareDrawModel()
{
	return true;
}
