#include "tmsimagecache_entry.h"

#include <QNetworkReply>

#include "../tmsimagecacheitem.h"

TmsImageCache::Entry::Entry() :
	status {Status::None},
	url {},
	lastAccess {0},
	item {nullptr},
	reply {nullptr}
{}

TmsImageCache::Entry::~Entry()
{
	delete item;
	delete reply;
}
