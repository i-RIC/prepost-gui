#ifndef TMSIMAGECACHE_ENTRY_H
#define TMSIMAGECACHE_ENTRY_H

#include "../tmsimagecache.h"

#include <QString>

class QNetworkReply;
class QPixmap;

class TmsImageCache::Entry
{
public:
	enum class Status {
		None,
		InQueue,
		Loading,
		CacheInMemory,
		CacheInFile,
	};

	Entry();
	~Entry();

	Status status;
	QString url;
	qint64 lastAccess;
	TmsImageCacheItem* item;
	QNetworkReply* reply;
};

#endif // TMSIMAGECACHE_ENTRY_H
