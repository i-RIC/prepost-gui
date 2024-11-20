#include "tmsimagecache_entry.h"

#include <QNetworkReply>
#include <QPixmap>

TmsImageCache::Entry::Entry() :
	status {Status::None},
	url {},
	lastAccess {0},
	pixmap {nullptr},
	reply {nullptr}
{}

TmsImageCache::Entry::~Entry()
{
	delete pixmap;
	delete reply;
}
