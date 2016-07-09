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

class QNetworkAccessManager;
class QWebView;

namespace tmsloader {

class TmsRequestHandler : public QObject
{
	Q_OBJECT
public:
	TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, double scale, const QString& templateName, int requestId, QNetworkAccessManager* manager, QObject *parent = nullptr);
	~TmsRequestHandler();

	int requestId() const;
	QImage image() const;

protected:
	void setArgs(const std::map<QString, QString>& args);
	void setup();

private slots:
	void checkImage();
	void handleLoaded();

signals:
	void imageUpdated();

private:
	QPointF m_center;
	QSize m_size;
	double m_scale;
	QString m_templateName;
	int m_requestId;

	std::map<QString, QString> m_args;
	QWebView* m_webView;
	QImage m_image;
	mutable QMutex m_imageMutex;

	QTimer m_timer;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLER_H
