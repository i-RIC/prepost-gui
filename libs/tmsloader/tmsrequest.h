#ifndef TMSLOADER_TMSREQUEST_H
#define TMSLOADER_TMSREQUEST_H

#include "tmsloader_api.h"

#include <QPixmap>
#include <QString>

#include <unordered_map>

class QPointF;
class QSize;
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
class QWebEngineView;
#else
class QWebView;
#endif
class QWidget;

class TmsImageCache;

namespace tmsloader {

class TmsRequestHandler;

class TMSLOADER_API TmsRequest
{
public:
	TmsRequest(const QPointF& centerLonLat, const QSize& size, double scale);
	virtual ~TmsRequest();

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	virtual TmsRequestHandler* buildHandler(int requestId, QWebEngineView* view, TmsImageCache* imageCache) const = 0;
#else
	virtual TmsRequestHandler* buildHandler(int requestId, QWebView* view, TmsImageCache* imageCache) const = 0;
#endif

protected:
	QPointF center() const;
	QSize size() const;
	double scale() const;

private:
	class Impl;
	Impl* impl;
};

} // tmsloader

#ifdef _DEBUG
	#include "private/tmsrequest_impl.h"
#endif // _DEBUG

#endif // TMSLOADER_TMSREQUEST_H
