#include "tmsimagecache.h"
#include "tmsimagecacheitem.h"
#include "tmsutil.h"
#include "private/tmsimagecache_entry.h"
#include "private/tmsimagecache_garbagecollectthread.h"
#include "private/tmsimagecache_networkaccessmanager.h"

#include <misc/filesystemfunction.h>

#include <gdal_priv.h>

#include <QDateTime>
#include <QUrl>

TmsImageCache::TmsImageCache() :
	m_tempDir {},
	m_dir {m_tempDir.path()},
	m_lonLat {false},
	m_manager {new NetworkAccessManager(this)},
	m_garbageCollectThread {new GarbageCollectThread(this)}
{
	GDALAllRegister();

	m_garbageCollectThread->start();
}

TmsImageCache::~TmsImageCache()
{
	delete m_manager;
	delete m_garbageCollectThread;
}

bool TmsImageCache::lonLat() const
{
	return m_lonLat;
}

void TmsImageCache::setLonLat(bool lonLat)
{
	m_lonLat = lonLat;
}

void TmsImageCache::addRequests(const QString& urlPattern, int zoomLevel, int xMin, int xMax, int yMin, int yMax, int maxZoomLevel)
{
	m_manager->clearRequestQueue();
	m_manager->addRequests(urlPattern, zoomLevel, xMin, xMax, yMin, yMax, maxZoomLevel);
}

bool TmsImageCache::exists(const QString& url) const
{
	auto it = m_entries.find(url);
	return it != m_entries.end();
}

TmsImageCacheItem* TmsImageCache::load(const QString& url)
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

			auto it2 = m_inMemoryEntries.find(url);
			if (it2 != m_inMemoryEntries.end()) {
				m_inMemoryEntries.erase(it2);
			}

			return nullptr;
		} else {
			QPixmap original, wgs84;
			QRectF wgs84Rect;
			QFile file(fname);
			file.open(QFile::ReadOnly);
			QDataStream stream(&file);
			stream >> original >> wgs84 >> wgs84Rect;
			int z, x, y;
			tmsloader::TmsUtil::getXYZ(url, &z, &x, &y);
			it->second->item = new TmsImageCacheItem(z, x, y, original, wgs84, wgs84Rect);

			m_inMemoryEntries.insert({url, it->second});
		}
	}
	it->second->lastAccess = QDateTime::currentMSecsSinceEpoch();
	return it->second->item;
}

void TmsImageCache::garbageCollect()
{
	m_garbageCollectThread->collect();
}

QString TmsImageCache::escapedUrl(const QString& url)
{
	QString ret = url;
	ret.replace(':', '_');
	ret.replace('/', '_');
	ret.replace('&', '_');

	return ret;
}

QString TmsImageCache::fileName(const QString& url) const
{
	return m_dir.absoluteFilePath(escapedUrl(url));
}
