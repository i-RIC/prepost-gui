#include "tmsimagecache.h"
#include "tmsrequesthandler.h"
#include "../tmsutil.h"

#include <cs/webmercatorutil.h>
#include <misc/qttool.h>

#include <QFile>
#include <QTextStream>
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
#include <QWebEngineView>
#else
#include <QWebView>
#endif
#include <QMutexLocker>
#include <QNetworkReply>
#include <QPainter>

using namespace tmsloader;

namespace {

const int TIMER_MSEC_SHORT = 10;
const int TIMER_MSEC_LONG = 200;

void calcSizeAndZoomLevel(const QSize& targetSize, double targetMeterPerPixel, const QPointF& center, QSize* size, int* zoomLevel)
{
	*zoomLevel = TmsUtil::calcNativeZoomLevel(center, targetMeterPerPixel) + 1;
	while (true) {
		-- *zoomLevel;
		double mpp = TmsUtil::meterPerPixel(center, *zoomLevel);
		double rate = targetMeterPerPixel / mpp;
		*size = QSize(targetSize.width() * rate, targetSize.height() * rate);

		if (rate < 1.0) {break;}
	}
}

} // namespace

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
TmsRequestHandler::TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, double scale, const QString& templateName, int requestId, QWebEngineView* view, TmsImageCache* imageCache) :
#else
TmsRequestHandler::TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, double scale, const QString& templateName, int requestId, QWebView* view, TmsImageCache* imageCache) :
#endif
	QObject {nullptr},
	m_center {centerLonLat},
	m_size {size},
	m_scale {scale},
	m_templateName {templateName},
	m_requestId {requestId},
	m_webView {view},
	m_terminating {false},
	m_webAccessManager {QtTool::networkAccessManager()},
	m_imageCache {imageCache},
	m_timer {this}
{
	// To see the view for debugging,, comment out the following line.
	// m_webView->move(0, 0);
}

TmsRequestHandler::~TmsRequestHandler()
{
	m_terminating = true;
	m_timer.stop();
	m_webView->stop();
}

int TmsRequestHandler::requestId() const
{
	return m_requestId;
}

QImage TmsRequestHandler::image() const
{
	QMutexLocker locker(&m_imageMutex);
	return m_image;
}

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
QWebEngineView* TmsRequestHandler::webView() const
#else
QWebView* TmsRequestHandler::webView() const
#endif
{
	return m_webView;
}

void TmsRequestHandler::setArgs(const std::map<QString, QString>& args)
{
	m_args = args;
}

void TmsRequestHandler::setOptions(const std::map<QString, QString>& options)
{
	m_options = options;
}

void TmsRequestHandler::setup()
{
	// calculate the appropriate window size and zoom level from m_size, m_scale, and m_center.
	QString url_template = m_args.at("%URL%");

	calcSizeAndZoomLevel(m_size, m_scale, m_center, &m_nativeSize, &m_zoomLevel);

	// calculate x, y at the center
	double x, y;
	WebMercatorUtil::project(m_center.x(), m_center.y(), &x, &y);
	auto zl = m_zoomLevel;
	auto maxZl = m_options.at("maxNativeZoom").toInt();
	if (zl > maxZl) {zl = maxZl;}

	int s = 1;
	for (int i = 0; i < m_zoomLevel; ++i) {
		s *= 2;
	}

	double lonMin, lonMax, latMin, latMax;
	WebMercatorUtil::unproject(
				x - m_nativeSize.width() / 2.0 / s, y + m_nativeSize.height() / 2.0 / s,
				&lonMin, &latMin);

	WebMercatorUtil::unproject(
				x + m_nativeSize.width() / 2.0 / s, y - m_nativeSize.height() / 2.0 / s,
				&lonMax, &latMax);

	WebMercatorUtil wmUtil(zl);
	wmUtil.getTileRegion(lonMin, latMax, lonMax, latMin, &m_xMin, &m_xMax, &m_yMin, &m_yMax);

	bool allImagesExists = true;
	for (int tileX = m_xMin; tileX <= m_xMax; ++tileX) {
		for (int tileY = m_yMin; tileY <= m_yMax; ++tileY) {
			QString url = url_template;
			url.replace("{z}", QString::number(zl));
			url.replace("{x}", QString::number(tileX));
			url.replace("{y}", QString::number(tileY));

			// auto httpUrl = url;
			// httpUrl.replace("https", "http");

			QUrl qUrl(url);
			// QUrl qUrl(httpUrl);

			if (m_imageCache->exists(qUrl.toString())) {continue;}

			allImagesExists = false;
			QNetworkRequest request(qUrl);

			auto reply = m_webAccessManager->get(request);
			m_networkReplies.insert(reply);
			connect(reply, &QNetworkReply::finished, this, &TmsRequestHandler::handleLoaded);
		}
	}

	m_image = QImage(m_size, QImage::Format_ARGB32);

	if (allImagesExists) {
		m_timer.singleShot(TIMER_MSEC_SHORT, this, &TmsRequestHandler::handleLoaded);
	} else {
		m_timer.singleShot(TIMER_MSEC_LONG, this, &TmsRequestHandler::handleLoaded);
	}
}

void TmsRequestHandler::handleLoaded()
{
	if (m_terminating) {return;}

	bool emitFlag = true;
	auto sndr = sender();
	if (sndr != nullptr) {
		auto reply = qobject_cast<QNetworkReply*> (sndr);
		if (reply != nullptr) {
			auto debugUrl = reply->url().toString();
			QPixmap pixmap;
			bool ok = pixmap.loadFromData(reply->readAll());
			m_imageCache->save(reply->url().toString(), pixmap);
			emitFlag = false;
		}
	}

	m_loading = false;

	QImage image(m_nativeSize, QImage::Format_ARGB32);

	QPainter painter;
	painter.begin(&image);
	painter.fillRect(0, 0, image.width(), image.height(), Qt::lightGray);

	double x, y;
	WebMercatorUtil::project(m_center.x(), m_center.y(), &x, &y);
	int s = 1;
	for (int i = 0; i < m_zoomLevel; ++i) {
		s *= 2;
	}

	long long scaledX = static_cast<long long> (x * s);
	long long scaledY = static_cast<long long> (y * s);

	auto zl = m_zoomLevel;
	auto maxZl = m_options.at("maxNativeZoom").toInt();
	if (zl > maxZl) {zl = maxZl;}

	int s2 = 1;
	if (m_zoomLevel > maxZl) {
		for (int i = 0; i < (m_zoomLevel - maxZl); ++i) {
			s2 *= 2;
		}
	}

	bool allImagesExists = true;
	QString url_template = m_args.at("%URL%");
	for (int tileX = m_xMin; tileX <= m_xMax; ++tileX) {
		for (int tileY = m_yMin; tileY <= m_yMax; ++tileY) {
			QString url = url_template;
			url.replace("{z}", QString::number(zl));
			url.replace("{x}", QString::number(tileX));
			url.replace("{y}", QString::number(tileY));

			// auto httpUrl = url;
			// httpUrl.replace("https", "http");

			// QUrl qUrl(httpUrl);
			QUrl qUrl(url);
			if (! m_imageCache->exists(qUrl.toString())) {
				allImagesExists = false;
				continue;
			}

			auto pixmap = m_imageCache->load(qUrl.toString());
			QPoint point;
			point.setX(tileX * 256.0 * s2 - scaledX + m_nativeSize.width() * 0.5);
			point.setY(tileY * 256.0 * s2 - scaledY + m_nativeSize.height() * 0.5);
			QRect pixmapRect;
			pixmapRect.setLeft(point.x());
			pixmapRect.setTop(point.y());
			pixmapRect.setRight(point.x() + pixmap.width() * s2 - 1);
			pixmapRect.setBottom(point.y() + pixmap.height() * s2 - 1);
			painter.drawPixmap(pixmapRect, pixmap, pixmap.rect());
		}
	}
	painter.end();

	m_imageMutex.lock();
	m_image = image.scaled(m_size);
	m_imageMutex.unlock();

	if (emitFlag) {
		emit imageUpdated();
	}

	if (! allImagesExists && emitFlag) {
		m_timer.singleShot(TIMER_MSEC_LONG, this, &TmsRequestHandler::handleLoaded);
	}
}

QString TmsRequestHandler::optionsString() const
{
	QStringList optionsStrs;
	for (auto it = m_options.begin(); it != m_options.end(); ++it) {
		optionsStrs.push_back(QString("%1: %2").arg(it->first).arg(it->second));
	}
	return optionsStrs.join(',');
}
