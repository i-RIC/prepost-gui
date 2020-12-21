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

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
TmsRequestHandler *TmsRequestXYZ::buildHandler(int requestId, QWebEngineView* view) const
#else
TmsRequestHandler *TmsRequestXYZ::buildHandler(int requestId, QWebView* view) const
#endif
{
	return new TmsRequestHandlerXYZ(impl->m_url, center(), size(), scale(), requestId, impl->m_options, view);
}
