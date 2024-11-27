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
		Loading,
		CacheInMemory,
		CacheInFile,
	};

	Entry();
	~Entry();

	Status status;
	QString url;
	qint64 lastAccess;
	QPixmap* pixmap;
	QNetworkReply* reply;
};

#endif // TMSIMAGECACHE_ENTRY_H
