#include "tmsimagecache.h"
#include "private/tmsimagecache_entry.h"
#include "private/tmsimagecache_garbagecollectthread.h"
#include "private/tmsimagecache_networkaccessmanager.h"

#include <misc/filesystemfunction.h>

#include <QDateTime>
#include <QUrl>

TmsImageCache::TmsImageCache() :
	m_tempDir {},
	m_dir {m_tempDir.path()},
	m_manager {new NetworkAccessManager(this)},
	m_garbageCollectThread {new GarbageCollectThread(this)}
{
	m_garbageCollectThread->start();
}

TmsImageCache::~TmsImageCache()
{
	delete m_manager;
	delete m_garbageCollectThread;
}

void TmsImageCache::addRequests(const QString& urlPattern, int zoomLevel, int xMin, int xMax, int yMin, int yMax, int maxZoomLevel)
{
	m_manager->addRequests(urlPattern, zoomLevel, xMin, xMax, yMin, yMax, maxZoomLevel);
}

bool TmsImageCache::exists(const QString& url) const
{
	auto it = m_entries.find(url);
	return it != m_entries.end();
}

QPixmap* TmsImageCache::load(const QString& url)
{
	QMutexLocker entriesLocker(&m_entriesMutex);

	auto it = m_entries.find(url);
	if (it == m_entries.end()) {return nullptr;}

	if (it->second->status == Entry::Status::None || it->second->status == Entry::Status::Loading) {return nullptr;}

	if (it->second->status == Entry::Status::CacheInFile) {
		auto fname = fileName(url);
		if (! QFile::exists(fileName(url))) {
			// return null image, and remove the entry.
			m_entries.erase(it);
			return nullptr;
		} else {
			auto pixmap = new QPixmap();
			pixmap->load(fname, "png");
			it->second->pixmap = pixmap;
		}
	}
	it->second->lastAccess = QDateTime::currentMSecsSinceEpoch();
	return it->second->pixmap;
}

void TmsImageCache::garbageCollect()
{
	m_garbageCollectThread->collect();
}

QString TmsImageCache::fileName(const QString& url) const
{
	QString localPath = url;
	localPath.replace(':', '_');
	localPath.replace('/', '_');
	localPath.replace('&', '_');

	return m_dir.absoluteFilePath(localPath);
}
