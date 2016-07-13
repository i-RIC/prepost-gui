#include "tmsrequestxyz.h"
#include "private/tmsrequestxyz_impl.h"
#include "private/tmsrequesthandlerxyz.h"

using namespace tmsloader;

TmsRequestXYZ::Impl::Impl(const QString &url, const std::map<QString, QString>& options) :
	m_url {url},
	m_options (options)
{}

// public interfaces

TmsRequestXYZ::TmsRequestXYZ(const QPointF& centerLonLat, const QSize& size, double scale, const QString& url, std::map<QString, QString>& options) :
	TmsRequest {centerLonLat, size, scale},
	impl {new Impl {url, options}}
{}

TmsRequestXYZ::~TmsRequestXYZ()
{
	delete impl;
}

TmsRequestHandler *TmsRequestXYZ::buildHandler(int requestId, QWebEngineView* view) const
{
	return new TmsRequestHandlerXYZ(impl->m_url, center(), size(), scale(), requestId, impl->m_options, view);
}
