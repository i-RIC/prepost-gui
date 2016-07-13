#include "tmsrequesthandler.h"

#include <QFile>
#include <QTextStream>
#include <QWebEngineView>
#include <QMutexLocker>

#include <cmath>

namespace {

const int TIMER_INTERVAL_MSEC = 100;

// Meter per pixel at equator for zoom level 1 = 40075334.2563 / 512.;
const double METERPERPIXEL_AT_EQUATOR_ZOOMLEVEL1 = 78272.137219;

void calcSizeAndZoomLevel(const QSize& targetSize, double targetMeterPerPixel, const QPointF& center, QSize* size, int* zoomLevel)
{
	*zoomLevel = 1;
	double meterPerPixel = METERPERPIXEL_AT_EQUATOR_ZOOMLEVEL1;
	meterPerPixel *= std::cos(center.y() / 180 * 3.141592);

	while (meterPerPixel > targetMeterPerPixel) {
		++ *zoomLevel;
		meterPerPixel /= 2.;
	}
	double rate = targetMeterPerPixel / meterPerPixel;
	*size = QSize(targetSize.width() * rate, targetSize.height() * rate);
}

} // namespace

using namespace tmsloader;

TmsRequestHandler::TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, double scale, const QString& templateName, int requestId, QWidget *parent) :
	QObject {parent},
	m_center {centerLonLat},
	m_size {size},
	m_scale {scale},
	m_templateName {templateName},
	m_requestId {requestId},
	m_webView {new QWebEngineView {parent}},
	m_timer {this}
{
	m_webView->move(parent->width() + 10, 0);

	// To see the view for debugging,, comment out the following line.
	// m_webView->move(0, 0);

	m_webView->resize(1, 1);
	m_webView->show();
}

TmsRequestHandler::~TmsRequestHandler()
{
	m_timer.stop();
	m_webView->stop();

	delete m_webView;
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

	for (auto pair : newArgs) {
		content.replace(pair.first, pair.second);
	}
	m_webView->setHtml(content);

	connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkImage()));
	m_timer.start(TIMER_INTERVAL_MSEC);
}

void TmsRequestHandler::checkImage()
{
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
	QImage image(m_webView->size(), QImage::Format_ARGB32);
	m_webView->render(&image);

	m_imageMutex.lock();
	m_image = image.scaled(m_size);
	m_imageMutex.unlock();

	emit imageUpdated();
}
