#include "tmsregionselectwidget.h"
#include "private/tmsregionselectwidget_impl.h"
#include "tmsimagesettingmanager.h"

#include <cs/coordinatesystem.h>
#include <tmsloader/tmsrequest.h>

#include <QMouseEvent>
#include <QPainter>
#include <QRectF>

TmsRegionSelectWidget::Impl::Impl(TmsRegionSelectWidget* w) :
	m_centerX {0},
	m_centerY {0},
	m_scale {4000},
	m_isSelected {false},
	m_point1 {0, 0},
	m_point2 {0, 0},
	m_coordinateSystem {nullptr},
	m_mapSetting (""),
	m_requestId {-1},
	m_loader {w},
	m_viewOperationState {ViewOperationState::None},
	m_widget {w}
{
	m_mapSetting = "tms=gsi&tiletype=std";

}

TmsRegionSelectWidget::Impl::~Impl()
{
	if (m_requestId != -1) {
		m_loader.cancelRequest(m_requestId);
	}
}

QRectF TmsRegionSelectWidget::Impl::regionRect() const
{
	double x1 = qMin(m_point1.x(), m_point2.x());
	double x2 = qMax(m_point1.x(), m_point2.x());
	double y1 = qMin(m_point1.y(), m_point2.y());
	double y2 = qMax(m_point1.y(), m_point2.y());

	QPointF topLeft, bottomRight;
	XYToPos(x1, y2, m_widget->size(), &topLeft);
	XYToPos(x2, y1, m_widget->size(), &bottomRight);

	return QRectF(topLeft.x(), topLeft.y(), (bottomRight.x() - topLeft.x()), (bottomRight.y() - topLeft.y()));
}

QRectF TmsRegionSelectWidget::Impl::imageRect() const
{
	QPointF topLeft, bottomRight;
	XYToPos(m_imageTopLeft.x(), m_imageTopLeft.y(), m_widget->size(), &topLeft);
	XYToPos(m_imageBottomRight.x(), m_imageBottomRight.y(), m_widget->size(), &bottomRight);

	return QRectF(topLeft.x(), topLeft.y(), (bottomRight.x() - topLeft.x()), (bottomRight.y() - topLeft.y()));
}

void TmsRegionSelectWidget::Impl::posToXY(const QPoint& pos, const QSize& size, double* x, double* y) const
{
	int cx = size.width()  / 2;
	int cy = size.height() / 2;
	double dx = pos.x() - cx;
	double dy = - (pos.y() - cy);

	*x = m_centerX + dx * m_scale;
	*y = m_centerY + dy * m_scale;
}

void TmsRegionSelectWidget::Impl::XYToPos(double x, double y, const QSize& size, QPointF* pos) const
{
	double dx = x - m_centerX;
	double dy = y - m_centerY;

	int cx = size.width()  / 2;
	int cy = size.height() / 2;

	*pos = QPointF(cx + dx / m_scale, cy - dy / m_scale);
}


// public interfaces

TmsRegionSelectWidget::TmsRegionSelectWidget(QWidget* parent) :
	QWidget(parent),
	impl {new Impl(this)}
{
	connect(&(impl->m_loader), SIGNAL(imageUpdated(int)), this, SLOT(handleImageUpdate(int)));
}

TmsRegionSelectWidget::~TmsRegionSelectWidget()
{
	delete impl;
}

void TmsRegionSelectWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

	painter.drawImage(impl->imageRect(), impl->m_image);

	painter.setPen(Qt::black);
	painter.drawRect(impl->regionRect());
}

void TmsRegionSelectWidget::resizeEvent(QResizeEvent *e)
{
	QWidget::resizeEvent(e);
	requestUpdate();
}

void TmsRegionSelectWidget::mousePressEvent(QMouseEvent *e)
{
	if (e->modifiers() == Qt::ControlModifier) {
		impl->m_previousPos = e->pos();
		if (e->button() == Qt::LeftButton) {
			impl->m_viewOperationState = Impl::ViewOperationState::Translating;
		} else if (e->button() == Qt::MidButton) {
			impl->m_viewOperationState = Impl::ViewOperationState::Zooming;
		}
		return;
	}
	impl->m_viewOperationState = Impl::ViewOperationState::None;

	double x, y;
	impl->posToXY(e->pos(), size(), &x, &y);

	impl->m_point1 = QPointF(x, y);
	impl->m_point2 = impl->m_point1;
	impl->m_isSelected = true;

	update();
}

void TmsRegionSelectWidget::mouseReleaseEvent(QMouseEvent *e)
{
	if (impl->m_viewOperationState != Impl::ViewOperationState::None) {
		requestUpdate();
	}
	impl->m_viewOperationState = Impl::ViewOperationState::None;
}

void TmsRegionSelectWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (impl->m_viewOperationState != Impl::ViewOperationState::None) {
		QPoint diff = e->pos() - impl->m_previousPos;

		if (impl->m_viewOperationState == Impl::ViewOperationState::Translating) {
			impl->m_centerX -= impl->m_scale * diff.x();
			impl->m_centerY += impl->m_scale * diff.y();
		} else if (impl->m_viewOperationState == Impl::ViewOperationState::Zooming) {
			impl->m_scale *= std::exp(diff.y() * 0.01);
		}
		update();
		impl->m_previousPos = e->pos();
		return;
	}

	double x, y;
	impl->posToXY(e->pos(), size(), &x, &y);

	impl->m_point2 = QPointF(x, y);

	update();
}

void TmsRegionSelectWidget::setCoordinateSystem(CoordinateSystem* cs)
{
	impl->m_coordinateSystem = cs;
	requestUpdate();
}

void TmsRegionSelectWidget::setMapSetting(const std::string& setting)
{
	impl->m_mapSetting = setting;
	requestUpdate();
}

void TmsRegionSelectWidget::setCenter(double x, double y)
{
	impl->m_centerX = x;
	impl->m_centerY = y;
	requestUpdate();
}

void TmsRegionSelectWidget::setScale(double scale)
{
	impl->m_scale = scale;
	requestUpdate();
}

bool TmsRegionSelectWidget::isSelected() const
{
	return impl->m_isSelected;
}

double TmsRegionSelectWidget::minLon() const
{
	double x = qMin(impl->m_point1.x(), impl->m_point2.x());
	double y = (impl->m_point1.y() + impl->m_point2.y()) * 0.5;

	double X, Y;
	impl->m_coordinateSystem->mapGridToGeo(x, y, &X, &Y);

	return X;
}

double TmsRegionSelectWidget::maxLon() const
{
	double x = qMax(impl->m_point1.x(), impl->m_point2.x());
	double y = (impl->m_point1.y() + impl->m_point2.y()) * 0.5;

	double X, Y;
	impl->m_coordinateSystem->mapGridToGeo(x, y, &X, &Y);

	return X;
}

double TmsRegionSelectWidget::minLat() const
{
	double x = (impl->m_point1.x() + impl->m_point2.x()) * 0.5;
	double y = qMin(impl->m_point1.y(), impl->m_point2.y());

	double X, Y;
	impl->m_coordinateSystem->mapGridToGeo(x, y, &X, &Y);

	return Y;
}

double TmsRegionSelectWidget::maxLat() const
{
	double x = (impl->m_point1.x() + impl->m_point2.x()) * 0.5;
	double y = qMax(impl->m_point1.y(), impl->m_point2.y());

	double X, Y;
	impl->m_coordinateSystem->mapGridToGeo(x, y, &X, &Y);

	return Y;
}

void TmsRegionSelectWidget::requestUpdate()
{
	if (impl->m_requestId != -1) {
		impl->m_loader.cancelRequest(impl->m_requestId);
	}

	if (impl->m_coordinateSystem == nullptr) {return;}

	TmsImageSettingManager manager;
	double lon, lat;
	impl->m_coordinateSystem->mapGridToGeo(impl->m_centerX, impl->m_centerY, &lon, &lat);

	if (lat > 82) {lat = 82;}
	if (lat < -82) {lat = -82;}

	tmsloader::TmsRequest* req = manager.buildRequest(QPointF(lon, lat), size(), impl->m_scale, impl->m_mapSetting);
	if (req == nullptr) {return;}

	impl->m_loader.registerRequest(*req, &impl->m_requestId);
	delete req;

	double x, y;
	QSize s = size();
	impl->posToXY(QPoint(0, 0), s, &x, &y);
	impl->m_imageTopLeft = QPointF(x, y);
	impl->posToXY(QPoint(s.width() - 1, s.height() - 1), s, &x, &y);
	impl->m_imageBottomRight = QPointF(x, y);
}

void TmsRegionSelectWidget::zoomIn()
{
	setScale(impl->m_scale * 0.5);
}

void TmsRegionSelectWidget::zoomOut()
{
	setScale(impl->m_scale * 2.0);
}

void TmsRegionSelectWidget::handleImageUpdate(int requestId)
{
	if (impl->m_requestId != requestId) {return;}

	impl->m_image = impl->m_loader.getImage(requestId);
	update();
}
