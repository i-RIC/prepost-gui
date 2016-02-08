#include "tmsrequestxyz.h"
#include "private/tmsrequestxyz_impl.h"
#include "private/tmsrequesthandlerxyz.h"

using namespace tmsloader;

TmsRequestXYZ::Impl::Impl(const QString &url) :
	m_url {url}
{}

// public interfaces

TmsRequestXYZ::TmsRequestXYZ(const QPointF& centerLonLat, const QSize& size, double scale, const QString& url) :
	TmsRequest {centerLonLat, size, scale},
	impl {new Impl {url}}
{}

TmsRequestXYZ::~TmsRequestXYZ()
{
	delete impl;
}

TmsRequestHandler *TmsRequestXYZ::buildHandler(int requestId, QNetworkAccessManager* manager, QObject* parent) const
{
	return new TmsRequestHandlerXYZ(impl->m_url, center(), size(), scale(), requestId, manager, parent);
}
