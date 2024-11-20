#ifndef TMSIMAGECACHE_H
#define TMSIMAGECACHE_H

#include <QDir>
#include <QMutex>
#include <QPixmap>
#include <QTemporaryDir>

#include <unordered_map>

class QString;

class TmsImageCache
{
public:
	TmsImageCache();
	~TmsImageCache();

	void addRequests(const QString& urlPattern, int zoomLevel, int xMin, int xMax, int yMin, int yMax, int maxZoomLevel);
	bool exists(const QString& url) const;
	QPixmap* load(const QString& url);
	void garbageCollect();

private:
	QString fileName(const QString& url) const;

	QTemporaryDir m_tempDir;
	QDir m_dir;

	class Request;

	QMutex m_entriesMutex;
	class Entry;
	std::unordered_map<QString, Entry*> m_entries;

	class NetworkAccessManager;
	NetworkAccessManager* m_manager;

	class GarbageCollectThread;
	GarbageCollectThread* m_garbageCollectThread;
};

#endif // TMSIMAGECACHE_H
