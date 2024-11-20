#ifndef TMSIMAGECACHE_NETWORKACCESSMANAGER_H
#define TMSIMAGECACHE_NETWORKACCESSMANAGER_H

#include "../tmsimagecache.h"

#include <QMutex>
#include <QObject>

class QNetworkAccessManager;

class TmsImageCache::NetworkAccessManager : public QObject
{
	Q_OBJECT

public:
	NetworkAccessManager(TmsImageCache* cache);
	~NetworkAccessManager();

	void addRequests(const QString& urlPattern, int zoomLevel, int xMin, int xMax, int yMin, int yMax, int maxZoomLevel);

private slots:
	void handleReply();

private:
	void registerRequest(const QString& urlPattern, int z, int x, int y);
	void registerRequestInAdvance(const QString& urlPattern, int z, int x, int y, int maxZoomLevel);
	void registerRequestWider(const QString& urlPattern, int z, int x, int y, int offset);
	void registerRequestLower(const QString& urlPattern, int z, int x, int y, int levels);
	void registerRequestUpper(const QString& urlPattern, int z, int x, int y, int maxZoomLevel, int levels);
	void doRegisterRequest(const QString& urlPattern, int z, int x, int y);

	QMutex m_requestsMutex;
	QNetworkAccessManager* m_networkAccessManager;

	TmsImageCache* m_cache;
	bool m_abort;
};

#endif // TMSIMAGECACHE_NETWORKACCESSMANAGER_H
