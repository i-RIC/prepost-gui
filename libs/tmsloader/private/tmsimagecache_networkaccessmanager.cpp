#include "../tmsutil.h"
#include "../tmsimagecacheitem.h"
#include "tmsimagecache_entry.h"
#include "tmsimagecache_networkaccessmanager.h"

#include <misc/qttool.h>

#include <QMutexLocker>
#include <QNetworkReply>

namespace {

const int MAX_REQUESTS = 20;

}

TmsImageCache::NetworkAccessManager::NetworkAccessManager(TmsImageCache* cache) :
	m_networkAccessManager {QtTool::networkAccessManager()},
	m_requestsInProgress {0},
	m_cache {cache},
	m_abort {false}
{}

TmsImageCache::NetworkAccessManager::~NetworkAccessManager()
{
	m_abort = true;
}

void TmsImageCache::NetworkAccessManager::addRequests(const QString& urlPattern, int zoomLevel, int xMin, int xMax, int yMin, int yMax, int maxZoomLevel)
{
	// register requests based on setting in request
	for (int x = xMin; x <= xMax; ++x) {
		for (int y = yMin; y <= yMax; ++y) {
			registerRequest(urlPattern, zoomLevel, x, y);
		}
	}

	// register requests in advance
	for (int x = xMin; x <= xMax; ++x) {
		for (int y = yMin; y <= yMax; ++y) {
			registerRequestInAdvance(urlPattern, zoomLevel, x, y, maxZoomLevel);
		}
	}
}

void TmsImageCache::NetworkAccessManager::clearRequestQueue()
{
	while (! m_requestQueue.empty()) {
		auto r = m_requestQueue.front();

		auto it = m_cache->m_entries.find(r.url().toString());
		if (it != m_cache->m_entries.end()) {
			QMutexLocker entriesLocker(&m_cache->m_entriesMutex);

			m_cache->m_entries.erase(it);
		}

		m_requestQueue.pop();
	}
}

void TmsImageCache::NetworkAccessManager::handleReply()
{
	if (m_abort) {return;}

	auto sndr = sender();
	if (sndr == nullptr) {return;}

	auto reply = qobject_cast<QNetworkReply*> (sndr);
	if (reply == nullptr) {return;}

	QPixmap pixmap;
	pixmap.loadFromData(reply->readAll());

	auto it = m_cache->m_entries.find(reply->url().toString());
	if (it != m_cache->m_entries.end()) {
		QMutexLocker entriesLocker(&m_cache->m_entriesMutex);

		int z, x, y;
		tmsloader::TmsUtil::getXYZ(reply->url().toString(), &z, &x, &y);

		it->second->item = new TmsImageCacheItem(z, x, y, pixmap, m_cache->lonLat());
		it->second->status = Entry::Status::CacheInMemory;

		m_cache->m_inMemoryEntries.insert({it->second->url, it->second});
	}

	// reply is needless
	it->second->reply = nullptr;
	delete reply;
	-- m_requestsInProgress;

	sendRequests();
}

void TmsImageCache::NetworkAccessManager::handleError(QNetworkReply::NetworkError)
{
	if (m_abort) {return;}

	auto sndr = sender();
	if (sndr == nullptr) {return;}

	auto reply = qobject_cast<QNetworkReply*> (sndr);
	if (reply == nullptr) {return;}

	auto it = m_cache->m_entries.find(reply->url().toString());
	if (it != m_cache->m_entries.end()) {
		QMutexLocker entriesLocker(&m_cache->m_entriesMutex);
		m_cache->m_entries.erase(it);
	}

	// reply is needless
	delete reply;
	-- m_requestsInProgress;

	sendRequests();
}

void TmsImageCache::NetworkAccessManager::registerRequest(const QString& urlPattern, int z, int x, int y)
{
	doRegisterRequest(urlPattern, z, x, y);
}

void TmsImageCache::NetworkAccessManager::registerRequestInAdvance(const QString& urlPattern, int z, int x, int y, int maxZoomLevel)
{
	registerRequestWider(urlPattern, z, x, y, 2);
	registerRequestLower(urlPattern, z, x, y, 2);
	registerRequestUpper(urlPattern, z, x, y, maxZoomLevel, 2);
}

void TmsImageCache::NetworkAccessManager::registerRequestWider(const QString& urlPattern, int z, int x, int y, int offset)
{
	for (int xx = x - offset; xx <= x + offset; ++xx) {
		for (int yy = y - offset; yy <= y + offset; ++yy) {
			doRegisterRequest(urlPattern, z, xx, yy);
		}
	}
}

void TmsImageCache::NetworkAccessManager::registerRequestLower(const QString& urlPattern, int z, int x, int y, int levels)
{
	if (levels == 0) {return;}

	int zz = z - 1;
	if (zz < 0) {return;}

	int xx = static_cast<int> (x / 2);
	int yy = static_cast<int> (y / 2);

	doRegisterRequest(urlPattern, zz, xx, yy);

	registerRequestLower(urlPattern, zz, xx, yy, levels - 1);
}

void TmsImageCache::NetworkAccessManager::registerRequestUpper(const QString& urlPattern, int z, int x, int y, int maxZoomLevel, int levels)
{
	if (levels == 0) {return;}

	int zz = z + 1;
	if (zz > maxZoomLevel) {return;}

	int xx = x * 2;
	int yy = y * 2;

	for (int dx = 0; dx < 2; ++dx) {
		int xxx = xx + dx;
		for (int dy = 0; dy < 2; ++dy) {
			int yyy = yy + dy;
			doRegisterRequest(urlPattern, zz, xxx, yyy);
			registerRequestUpper(urlPattern, zz, xx, yy, maxZoomLevel, levels - 1);
		}
	}
}

void TmsImageCache::NetworkAccessManager::doRegisterRequest(const QString& urlPattern, int z, int x, int y)
{
	QString url = urlPattern;
	url.replace("{z}", QString::number(z));
	url.replace("{x}", QString::number(x));
	url.replace("{y}", QString::number(y));

	// auto httpUrl = url;
	// httpUrl.replace("https", "http");

	QUrl qUrl(url);
	// QUrl qUrl(httpUrl);

	if (m_cache->exists(qUrl.toString())) {return;}

	QNetworkRequest request(qUrl);
	request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36");
	m_requestQueue.push(request);

	auto entry = new Entry();
	entry->status = Entry::Status::InQueue;
	entry->url = qUrl.toString();

	m_cache->m_entriesMutex.lock();
	m_cache->m_entries.insert({qUrl.toString(), entry});
	m_cache->m_entriesMutex.unlock();

	sendRequests();
}

void TmsImageCache::NetworkAccessManager::sendRequests()
{
	if (m_requestsInProgress >= MAX_REQUESTS) {return;}

	auto requestsToSend = std::min(MAX_REQUESTS - m_requestsInProgress, static_cast<int> (m_requestQueue.size()));
	for (int i = 0; i < requestsToSend; ++i) {
		auto r = m_requestQueue.front();

		auto it = m_cache->m_entries.find(r.url().toString());
		if (it != m_cache->m_entries.end()) {
			QMutexLocker entriesLocker(&m_cache->m_entriesMutex);

			auto entry = it->second;
			entry->reply = m_networkAccessManager->get(r);
			connect(entry->reply, &QNetworkReply::finished, this, &NetworkAccessManager::handleReply);
			connect<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(entry->reply, &QNetworkReply::error, this, &NetworkAccessManager::handleError);
		}
		++ m_requestsInProgress;
		m_requestQueue.pop();
	}
}
