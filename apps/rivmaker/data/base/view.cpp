#include "model.h"
#include "view.h"
#include "dataitemcontroller.h"

#include <QColor>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>

const int View::CLICK_LIMIT = 3;
const int View::NEAR_LIMIT = 5;

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
	auto m = model();
	if (m == nullptr) {return;}
	auto r = m->rootDataItemView();
	if (r == nullptr) {return;}
	if (! r->prepareDraw()) {return;}

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

void View::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(rect(), QColor(250, 250, 250));

	auto iv = model()->rootDataItemView();
	if (iv == nullptr) {return;}

	iv->draw(&painter);
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
	QPointF p = rconv(QPointF(event->pos()));
	emit positionChanged(p);

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

	QMenu menu(this);
	c->setupViewRightClickMenu(&menu);
	if (m_model->selectedItem()->isDeletable()) {
		menu.addSeparator();
		menu.addAction(model()->deleteAction());
	}
	if (menu.actions().size() == 0) {return;}

	menu.exec(event->globalPos());
}

void View::wheelEvent(QWheelEvent* event)
{
	viewWheelEvent(event);
}

void View::resizeEvent(QResizeEvent*)
{
	updateView();
}

void View::paramsResetRotation()
{}

void View::paramsRotate90()
{}

void View::emitPosition(QMouseEvent*)
{

}

void View::setupCursors()
{
	m_zoomPixmap = QPixmap(":/images/cursorZoom.png");
	m_movePixmap = QPixmap(":/images/cursorMove.png");
	m_rotatePixmap = QPixmap(":/images/cursorRotate.png");

	m_zoomCursor = QCursor(m_zoomPixmap, 16, 16);
	m_moveCursor = QCursor(m_movePixmap, 16, 16);
	m_rotateCursor = QCursor(m_rotatePixmap, 16, 16);
}

bool View::prepareDrawModel()
{
	return true;
}
