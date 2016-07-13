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

TmsRequestHandler *TmsRequestGoogleMap::buildHandler(int requestId, QWebEngineView* view) const
{
	return new TmsRequestHandlerGoogleMap(impl->m_mapType, center(), size(), scale(), requestId, view);
}
