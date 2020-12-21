#include "tmsrequestbing.h"
#include "private/tmsrequestbing_impl.h"
#include "private/tmsrequesthandlerbing.h"

using namespace tmsloader;

TmsRequestBing::Impl::Impl(TmsRequestBing::ImagerySet imagerySet) :
	m_imagerySet {imagerySet}
{}

// public interfaces

TmsRequestBing::TmsRequestBing(const QPointF& centerLonLat, const QSize& size, double scale, ImagerySet imagerySet) :
	TmsRequest {centerLonLat, size, scale},
	impl {new Impl {imagerySet}}
{}

TmsRequestBing::~TmsRequestBing()
{
	delete impl;
}

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
TmsRequestHandler* TmsRequestBing::buildHandler(int requestId, QWebEngineView* view) const
#else
TmsRequestHandler* TmsRequestBing::buildHandler(int requestId, QWebView* view) const
#endif
{
	return new TmsRequestHandlerBing(impl->m_imagerySet, center(), size(), scale(), requestId, view);
}
