#include "tmsrequesthandler.h"
#include "../tmsutil.h"

#include <QFile>
#include <QTextStream>
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
#include <QWebEngineView>
#else
#include <QWebView>
#endif
#include <QMutexLocker>

using namespace tmsloader;

namespace {

const int TIMER_INTERVAL_MSEC = 100;

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
TmsRequestHandler::TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, double scale, const QString& templateName, int requestId, QWebEngineView* view) :
#else
TmsRequestHandler::TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, double scale, const QString& templateName, int requestId, QWebView* view) :
#endif
	QObject {nullptr},
	m_center {centerLonLat},
	m_size {size},
	m_scale {scale},
	m_templateName {templateName},
	m_requestId {requestId},
	m_webView {view},
	m_terminating {false},
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
	QSize size;
	int zoomLevel;

	calcSizeAndZoomLevel(m_size, m_scale, m_center, &size, &zoomLevel);

	m_webView->resize(size);

	connect(m_webView, SIGNAL(loadFinished(bool)), this, SLOT(handleLoaded()));

	QFile file(QString(":/data/%1").arg(m_templateName));
	file.open(QFile::ReadOnly | QFile::Text);
	QTextStream in(&file);

	QString content = in.readAll();

	std::map<QString, QString> newArgs = m_args;

	newArgs.insert({"%LONGITUDE%", QString::number(m_center.x(), 'g', 10)});
	newArgs.insert({"%LATITUDE%", QString::number(m_center.y(), 'g', 10)});
	newArgs.insert({"%ZOOMLEVEL%", QString::number(zoomLevel)});
	newArgs.insert({"%WIDTH%", QString::number(size.width())});
	newArgs.insert({"%HEIGHT%", QString::number(size.height())});

	m_options.insert({"maxZoom", QString::number(zoomLevel)});
	newArgs.insert({"%OPTIONS%", optionsString()});

	for (auto pair : newArgs) {
		content.replace(pair.first, pair.second);
	}
	m_webView->setHtml(content);
	m_loading = true;

	m_image = QImage(size.width(), size.height(), QImage::Format_ARGB32);
	emit imageUpdated();

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkImage()));
	m_timer.start(TIMER_INTERVAL_MSEC);
}

void TmsRequestHandler::checkImage()
{
	if (m_loading == true) {return;}

	QImage newImage(m_webView->size(), QImage::Format_ARGB32);
	m_webView->render(&newImage);

	if (newImage == m_image) {return;}

	m_imageMutex.lock();
	m_image = newImage.scaled(m_size);
	m_imageMutex.unlock();

	emit imageUpdated();
}

void TmsRequestHandler::handleLoaded()
{
	if (m_terminating) {return;}

	m_loading = false;

	QImage image(m_webView->size(), QImage::Format_ARGB32);
	m_webView->render(&image);

	m_imageMutex.lock();
	m_image = image.scaled(m_size);
	m_imageMutex.unlock();

	emit imageUpdated();
}

QString TmsRequestHandler::optionsString() const
{
	QStringList optionsStrs;
	for (auto it = m_options.begin(); it != m_options.end(); ++it) {
		optionsStrs.push_back(QString("%1: %2").arg(it->first).arg(it->second));
	}
	return optionsStrs.join(',');
}
