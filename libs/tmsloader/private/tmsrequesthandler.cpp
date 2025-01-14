#include "tmsimagecache.h"
#include "tmsrequesthandler.h"
#include "../tmsutil.h"

#include <cs/webmercatorutil.h>
#include <misc/qttool.h>

#include <QFile>
#include <QTextStream>
#include <QMutexLocker>
#include <QNetworkReply>
#include <QPainter>

using namespace tmsloader;

namespace {

const int TIMER_MSEC_SHORT = 10;
const int TIMER_MSEC_LONG = 200;

} // namespace

TmsRequestHandler::TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, int zoomLevel, const QString& templateName, int requestId, TmsImageCache* imageCache) :
	QObject {nullptr},
	m_center {centerLonLat},
	m_size {size},
	m_zoomLevel {zoomLevel},
	m_templateName {templateName},
	m_requestId {requestId},
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
				x - m_size.width() / 2.0 / s, y + m_size.height() / 2.0 / s,
				&lonMin, &latMin);

	WebMercatorUtil::unproject(
				x + m_size.width() / 2.0 / s, y - m_size.height() / 2.0 / s,
				&lonMax, &latMax);

	WebMercatorUtil wmUtil(zl);
	wmUtil.getTileRegion(lonMin, latMax, lonMax, latMin, &m_xMin, &m_xMax, &m_yMin, &m_yMax);

	m_imageCache->addRequests(url_template, zl, m_xMin, m_xMax, m_yMin, m_yMax, maxZl);

	m_image = QImage(m_size, QImage::Format_ARGB32);

	m_timer.singleShot(TIMER_MSEC_SHORT, this, &TmsRequestHandler::handleLoaded);
}

void TmsRequestHandler::handleLoaded()
{
	if (m_terminating) {return;}

	bool emitFlag = true;
	QImage image(m_size, QImage::Format_ARGB32);

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

			auto pixmap = m_imageCache->load(qUrl.toString());
			if (pixmap == nullptr) {
				allImagesExists = false;
				continue;
			}

			QPoint point;
			point.setX(tileX * 256.0 * s2 - scaledX + m_size.width() * 0.5);
			point.setY(tileY * 256.0 * s2 - scaledY + m_size.height() * 0.5);
			QRect pixmapRect;
			pixmapRect.setLeft(point.x());
			pixmapRect.setTop(point.y());
			pixmapRect.setRight(point.x() + pixmap->width() * s2 - 1);
			pixmapRect.setBottom(point.y() + pixmap->height() * s2 - 1);
			painter.drawPixmap(pixmapRect, *pixmap, pixmap->rect());
		}
	}
	painter.end();

	m_imageMutex.lock();
	m_image = image;
	m_imageMutex.unlock();

	if (emitFlag) {
		emit imageUpdated();
	}

	if (! allImagesExists && emitFlag) {
		m_timer.singleShot(TIMER_MSEC_LONG, this, &TmsRequestHandler::handleLoaded);
	}

	if (allImagesExists) {
		m_imageCache->garbageCollect();
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
