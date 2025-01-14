#ifndef TMSIMAGECACHE_GARBAGECOLLECTTHREAD_H
#define TMSIMAGECACHE_GARBAGECOLLECTTHREAD_H

#include "../tmsimagecache.h"

#include <QMutex>
#include <QThread>

class TmsImageCache::GarbageCollectThread : public QThread
{
public:
	GarbageCollectThread(TmsImageCache* cache);
	~GarbageCollectThread();

	void collect();

private:
	void run() override;

	bool m_collect;
	QMutex m_collectMutex;
	TmsImageCache* m_cache;

	bool m_abort;
};

#endif // TMSIMAGECACHE_GARBAGECOLLECTTHREAD_H
