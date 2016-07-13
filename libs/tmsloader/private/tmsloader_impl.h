#ifndef TMSLOADER_TMSLOADER_IMPL_H
#define TMSLOADER_TMSLOADER_IMPL_H

#include "../tmsloader.h"

#include <vector>

class QWebEngineView;
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

	QWebEngineView* getWebView();

	std::vector<TmsRequestHandler*> m_handlers;
	std::vector<QWebEngineView*> m_webViewPool;

	TmsLoader* m_loader;
};

} // tmsloader

#endif // TMSLOADER_TMSLOADER_IMPL_H
