#include "../tmsimagecacheitem.h"
#include "tmsimagecache_entry.h"
#include "tmsimagecache_garbagecollectthread.h"

#include <QMutexLocker>
#include <QDataStream>

#include <map>

namespace {

const int COLLECT_THRESHOLD = 1200;
const int MAX_IMAGES = 800;

} // namespace

TmsImageCache::GarbageCollectThread::GarbageCollectThread(TmsImageCache* cache) :
	QThread {},
	m_collect {false},
	m_cache {cache},
	m_abort {false}
{}

TmsImageCache::GarbageCollectThread::~GarbageCollectThread()
{
	m_abort = true;
	wait();
}

void TmsImageCache::GarbageCollectThread::collect()
{
	QMutexLocker locker(&m_collectMutex);
	m_collect = true;
}

void TmsImageCache::GarbageCollectThread::run()
{
	forever {
		if (m_abort) {return;}

		if (! m_collect) {
			msleep(500);
			continue;
		}

		if (m_cache->m_inMemoryEntries.size() > COLLECT_THRESHOLD) {
			std::multimap<qint64, QString> epochs;

			m_cache->m_entriesMutex.lock();
			for (const auto& pair : m_cache->m_inMemoryEntries) {
				const auto& url = pair.first;
				const qint64 epoch = pair.second->lastAccess;
				epochs.insert({epoch, url});
			}
			m_cache->m_entriesMutex.unlock();

			auto epochs_it = epochs.begin();
			unsigned int i = 0;
			unsigned int imax = m_cache->m_inMemoryEntries.size() - MAX_IMAGES;
			while (true) {
				m_cache->m_entriesMutex.lock();
				auto entries_it = m_cache->m_inMemoryEntries.find(epochs_it->second);
				m_cache->m_entriesMutex.unlock();

				auto fname = m_cache->fileName(entries_it->first);
				QFile file(fname);
				file.open(QFile::WriteOnly);
				QDataStream stream(&file);
				const auto& item = entries_it->second->item;
				stream << item->original() << item->wgs84() << item->wgs84Rect();
				file.close();

				m_cache->m_entriesMutex.lock();
				delete entries_it->second->item;
				entries_it->second->item = nullptr;
				entries_it->second->status = Entry::Status::CacheInFile;
				m_cache->m_inMemoryEntries.erase(entries_it);
				m_cache->m_entriesMutex.unlock();

				++ epochs_it;
				++ i;
				if (i >= imax) {break;}
				if (epochs_it == epochs.end()) {break;}
			}
		}

		m_collect = false;
	}
}
