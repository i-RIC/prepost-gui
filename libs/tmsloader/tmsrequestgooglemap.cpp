#include "tmsrequestgooglemap.h"
#include "private/tmsrequestgooglemap_impl.h"
#include "private/tmsrequesthandlergooglemap.h"

using namespace tmsloader;

TmsRequestGoogleMap::Impl::Impl(TmsRequestGoogleMap::MapType mapType) :
	m_mapType {mapType}
{}

TmsRequestGoogleMap::TmsRequestGoogleMap(const QPointF& centerLonLat, const QSize& size, double scale, MapType mapType) :
	TmsRequest {centerLonLat, size, scale},
	impl {new Impl {mapType}}
{}

TmsRequestGoogleMap::~TmsRequestGoogleMap()
{
	delete impl;
}

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
TmsRequestHandler *TmsRequestGoogleMap::buildHandler(int requestId, QWebEngineView* view) const
#else
TmsRequestHandler *TmsRequestGoogleMap::buildHandler(int requestId, QWebView* view) const
#endif
{
	return new TmsRequestHandlerGoogleMap(impl->m_mapType, center(), size(), scale(), requestId, view);
}
