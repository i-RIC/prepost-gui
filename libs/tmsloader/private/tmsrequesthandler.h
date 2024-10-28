#ifndef TMSLOADER_TMSREQUESTHANDLER_H
#define TMSLOADER_TMSREQUESTHANDLER_H

#include <QImage>
#include <QMutex>
#include <QObject>
#include <QPointF>
#include <QSize>
#include <QString>
#include <QTimer>

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

class QNetworkAccessManager;
class QNetworkReply;
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
class QWebEngineView;
#else
class QWebView;
#endif
class TmsImageCache;
class QWidget;

namespace tmsloader {

class TmsRequestHandler : public QObject
{
	Q_OBJECT

public:
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, double scale, const QString& templateName, int requestId, QWebEngineView* view, TmsImageCache* imageCache);
#else
	TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, double scale, const QString& templateName, int requestId, QWebView* view, TmsImageCache* imageCache);
#endif
	~TmsRequestHandler();

	int requestId() const;
	QImage image() const;
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	QWebEngineView* webView() const;
#else
	QWebView* webView() const;
#endif

protected:
	void setArgs(const std::map<QString, QString>& args);
	void setOptions(const std::map<QString, QString>& options);
	void setup();

private slots:
	void handleLoaded();

signals:
	void imageUpdated();

private:
	QString optionsString() const;

	QPointF m_center;
	QSize m_size;
	double m_scale;
	QString m_templateName;
	int m_requestId;

	std::map<QString, QString> m_args;
	std::map<QString, QString> m_options;

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	QWebEngineView* m_webView;
#else
	QWebView* m_webView;
#endif
	QImage m_image;
	mutable QMutex m_imageMutex;
	bool m_terminating;
	bool m_loading;

	QNetworkAccessManager* m_webAccessManager;
	TmsImageCache* m_imageCache;
	std::unordered_set<QNetworkReply*> m_networkReplies;
	QSize m_nativeSize;
	int m_zoomLevel;
	int m_xMin;
	int m_xMax;
	int m_yMin;
	int m_yMax;

	QTimer m_timer;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLER_H
