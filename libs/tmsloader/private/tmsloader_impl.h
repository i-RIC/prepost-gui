#ifndef TMSLOADER_TMSLOADER_IMPL_H
#define TMSLOADER_TMSLOADER_IMPL_H

#include "../tmsloader.h"

#include <vector>

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
class QWebEngineView;
#else
class QWebView;
#endif
class QWidget;

namespace tmsloader {

class TmsRequestHandler;

class TmsLoader::Impl
{
public:
	Impl(TmsLoader* parent);
	~Impl();

	TmsRequestHandler* registerNewHandler(const TmsRequest& request);

	QWidget* parentWidget() const;

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	QWebEngineView* getWebView();
#else
	QWebView* getWebView();
#endif

	std::vector<TmsRequestHandler*> m_handlers;
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	std::vector<QWebEngineView*> m_webViewPool;
#else
	std::vector<QWebView*> m_webViewPool;
#endif

	TmsLoader* m_loader;
};

} // tmsloader

#endif // TMSLOADER_TMSLOADER_IMPL_H
