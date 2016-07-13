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

TmsRequestHandler* TmsRequestBing::buildHandler(int requestId, QWebEngineView* view) const
{
	return new TmsRequestHandlerBing(impl->m_imagerySet, center(), size(), scale(), requestId, view);
}
