#include "tmsregionselectwidget.h"
#include "private/tmsregionselectwidget_impl.h"
#include "tmsimagesettingmanager.h"

#include <cs/webmercatorutil.h>
#include <tmsloader/tmsrequest.h>
#include <tmsloader/tmsutil.h>

#include <QMouseEvent>
#include <QPainter>
#include <QRectF>

TmsRegionSelectWidget::Impl::Impl(TmsRegionSelectWidget* w) :
	m_centerLon {0},
	m_centerLat {0},
	m_zoomLevel {4},
	m_isSelected {false},
	m_pointLonLat1 {0, 0},
	m_pointLonLat2 {0, 0},
	m_mapSetting (""),
	m_requestId {-1},
	m_loader {w},
	m_viewOperationState {ViewOperationState::None},
	m_movePixmap(":/libs/guibase/images/cursorMove.png"),
	m_zoomPixmap(":/libs/guibase/images/cursorZoom.png"),
	m_moveCursor(m_movePixmap),
	m_zoomCursor(m_zoomPixmap),
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
	double lonMin = qMin(m_pointLonLat1.x(), m_pointLonLat2.x());
	double lonMax = qMax(m_pointLonLat1.x(), m_pointLonLat2.x());
	double latMin = qMin(m_pointLonLat1.y(), m_pointLonLat2.y());
	double latMax = qMax(m_pointLonLat1.y(), m_pointLonLat2.y());

	double xMin, xMax, yMin, yMax;

	lonLatToXY(m_widget->size(), lonMin, latMax, &xMin, &yMin);
	lonLatToXY(m_widget->size(), lonMax, latMin, &xMax, &yMax);

	return QRectF(xMin, yMin, (xMax - xMin), (yMax - yMin));
}

QRectF TmsRegionSelectWidget::Impl::imageRect() const
{
	double xMin, xMax, yMin, yMax;

	lonLatToXY(m_widget->size(), m_imageLonMin, m_imageLatMax, &xMin, &yMin);
	lonLatToXY(m_widget->size(), m_imageLonMax, m_imageLatMin, &xMax, &yMax);

	return QRectF(xMin, yMin, (xMax - xMin), (yMax - yMin));
}

void TmsRegionSelectWidget::Impl::lonLatToXY(const QSize& size, double lon, double lat, double* x, double* y) const
{
	double centerX, centerY, px, py;

	WebMercatorUtil::project(m_centerLon, m_centerLat, &centerX, &centerY);
	WebMercatorUtil::project(lon, lat, &px, &py);
	double rate = WebMercatorUtil::pixelSize(m_zoomLevel);

	*x = size.width()  / 2 + (px - centerX) / rate;
	*y = size.height() / 2 + (py - centerY) / rate;
}

void TmsRegionSelectWidget::Impl::XYToLonLat(const QSize& size, double x, double y, double* lon, double* lat) const
{
	double centerX, centerY, px, py;

	WebMercatorUtil::project(m_centerLon, m_centerLat, &centerX, &centerY);
	double rate = WebMercatorUtil::pixelSize(m_zoomLevel);

	px = centerX + (x - size.width()  / 2) * rate;
	py = centerY + (y - size.height() / 2) * rate;

	WebMercatorUtil::unproject(px, py, lon, lat);
}

// public interfaces

TmsRegionSelectWidget::TmsRegionSelectWidget(QWidget* parent) :
	QWidget(parent),
	impl {new Impl(this)}
{
	connect(&(impl->m_loader), SIGNAL(imageUpdated(int)), this, SLOT(handleImageUpdate(int)));
	impl->m_timer.start(10);
	connect(&(impl->m_timer), SIGNAL(timeout()), this, SLOT(handleTimer()));
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
			setCursor(impl->m_moveCursor);
		} else if (e->button() == Qt::MidButton) {
			impl->m_viewOperationState = Impl::ViewOperationState::Zooming;
			setCursor(impl->m_zoomCursor);
		}
		return;
	}
	impl->m_viewOperationState = Impl::ViewOperationState::None;

	double lon, lat;
	impl->XYToLonLat(size(), e->x(), e->y(), &lon, &lat);

	impl->m_pointLonLat1 = QPointF(lon, lat);
	impl->m_pointLonLat2 = impl->m_pointLonLat1;
	impl->m_isSelected = true;

	update();
}

void TmsRegionSelectWidget::mouseReleaseEvent(QMouseEvent * /*e*/)
{
	if (impl->m_viewOperationState != Impl::ViewOperationState::None) {
		requestUpdate();
	}
	impl->m_viewOperationState = Impl::ViewOperationState::None;
	unsetCursor();
}

void TmsRegionSelectWidget::mouseMoveEvent(QMouseEvent *e)
{
	if (impl->m_viewOperationState != Impl::ViewOperationState::None) {
		QPoint diff = e->pos() - impl->m_previousPos;

		if (impl->m_viewOperationState == Impl::ViewOperationState::Translating) {
			double centerX, centerY, rate;

			impl->lonLatToXY(size(), impl->m_centerLon, impl->m_centerLat, &centerX, &centerY);
			centerX -= diff.x();
			centerY -= diff.y();
			impl->XYToLonLat(size(), centerX, centerY, &(impl->m_centerLon), &(impl->m_centerLat));
		} else if (impl->m_viewOperationState == Impl::ViewOperationState::Zooming) {
			impl->m_zoomLevel -= diff.y() * 0.01;
		}
		update();
		impl->m_previousPos = e->pos();
		return;
	}

	double lon, lat;
	impl->XYToLonLat(size(), e->x(), e->y(), &lon, &lat);
	impl->m_pointLonLat2 = QPointF(lon, lat);

	update();
}

void TmsRegionSelectWidget::wheelEvent(QWheelEvent* event)
{
	int delta = event->delta() / QWheelEvent::DefaultDeltasPerStep;
	double zoomRate = 1.2;
	if (delta < 0) {
		delta = - delta;
		zoomRate = 1 / zoomRate;
	}
	while (delta > 0) {
		impl->m_zoomLevel *= zoomRate;
		-- delta;
	}

	update();
	requestUpdate();
}

void TmsRegionSelectWidget::setMapSetting(const std::string& setting)
{
	impl->m_mapSetting = setting;
	requestUpdate();
}

void TmsRegionSelectWidget::setCenter(double lon, double lat)
{
	impl->m_centerLon = lon;
	impl->m_centerLat = lat;
	requestUpdate();
}

void TmsRegionSelectWidget::setZoomLevel(double zoomLevel)
{
	impl->m_zoomLevel = zoomLevel;
	requestUpdate();
}

bool TmsRegionSelectWidget::isSelected() const
{
	return impl->m_isSelected;
}

double TmsRegionSelectWidget::minLon() const
{
	return qMin(impl->m_pointLonLat1.x(), impl->m_pointLonLat2.x());
}

double TmsRegionSelectWidget::maxLon() const
{
	return qMax(impl->m_pointLonLat1.x(), impl->m_pointLonLat2.x());
}

double TmsRegionSelectWidget::minLat() const
{
	return qMin(impl->m_pointLonLat1.y(), impl->m_pointLonLat2.y());
}

double TmsRegionSelectWidget::maxLat() const
{
	return qMax(impl->m_pointLonLat1.y(), impl->m_pointLonLat2.y());
}

void TmsRegionSelectWidget::requestUpdate()
{
	static bool updating = false;
	if (updating) {return;}

	if (impl->m_requestId != -1) {
		impl->m_loader.cancelRequest(impl->m_requestId);
		impl->m_requestId = -1;
	}

	TmsImageSettingManager manager;

	QPointF center(impl->m_centerLon, impl->m_centerLat);
	double scale = tmsloader::TmsUtil::meterPerPixel(center, impl->m_zoomLevel);
	tmsloader::TmsRequest* req = manager.buildRequest(center, size(), scale, impl->m_mapSetting);
	if (req == nullptr) {return;}

	int rId;
	updating = true;
	impl->m_loader.registerRequest(*req, &rId);
	updating = false;
	if (impl->m_requestId != -1) {
		impl->m_loader.cancelRequest(impl->m_requestId);
	}
	impl->m_requestId = rId;

	delete req;

	QSize s = size();
	impl->XYToLonLat(s, 0, 0, &(impl->m_imageLonMin), &(impl->m_imageLatMax));
	impl->XYToLonLat(s, s.width(), s.height(), &(impl->m_imageLonMax), &(impl->m_imageLatMin));
}

void TmsRegionSelectWidget::zoomIn()
{
	setZoomLevel(impl->m_zoomLevel + 1);
}

void TmsRegionSelectWidget::zoomOut()
{
	setZoomLevel(impl->m_zoomLevel - 1);
}

void TmsRegionSelectWidget::handleImageUpdate(int requestId)
{
	if (impl->m_requestId != requestId) {return;}

	impl->m_image = impl->m_loader.getImage(requestId);
	update();
}

void TmsRegionSelectWidget::handleTimer()
{
	requestUpdate();
	impl->m_timer.stop();
}

