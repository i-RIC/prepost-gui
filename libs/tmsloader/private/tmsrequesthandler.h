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
class TmsImageCache;
class QWidget;

namespace tmsloader {

class TmsRequestHandler : public QObject
{
	Q_OBJECT

public:
	TmsRequestHandler(const QPointF& centerLonLat, const QSize& size, int zoomLevel, const QString& templateName, int requestId, TmsImageCache* imageCache);
	~TmsRequestHandler();

	int requestId() const;
	QImage image() const;
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
	int m_zoomLevel;
	QString m_templateName;
	int m_requestId;

	std::map<QString, QString> m_args;
	std::map<QString, QString> m_options;

	QImage m_image;
	mutable QMutex m_imageMutex;
	bool m_terminating;

	QNetworkAccessManager* m_webAccessManager;
	TmsImageCache* m_imageCache;
	int m_xMin;
	int m_xMax;
	int m_yMin;
	int m_yMax;

	QTimer m_timer;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLER_H
