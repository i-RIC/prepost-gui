#include "tmsloader.h"
#include "tmsrequest.h"
#include "private/tmsloader_impl.h"
#include "private/tmsrequesthandler.h"

#include <QPixmap>
#include <QWebEngineView>
#include <QWidget>

using namespace tmsloader;

namespace {
	const int WINDOW_BIG_WIDTH = 10000;
}

TmsLoader::Impl::Impl(TmsLoader* parent) :
	m_loader (parent)
{}

TmsLoader::Impl::~Impl()
{
	for (auto handler : m_handlers) {
		delete handler;
	}
	for (auto v : m_webViewPool) {
		delete v;
	}
}

TmsRequestHandler *TmsLoader::Impl::registerNewHandler(const TmsRequest &request)
{
	// setup new Id;
	int newId = -1;
	for (int i = 0; i < m_handlers.size(); ++i) {
		if (m_handlers.at(i) == nullptr) {
			newId = i;
			break;
		}
	}

	if (newId == -1) {
		m_handlers.push_back(nullptr);
		newId = static_cast<int>(m_handlers.size()) - 1;
	}

	TmsRequestHandler* handler = request.buildHandler(newId, getWebView());
	m_handlers[newId] = handler;

	return handler;
}

QWidget* TmsLoader::Impl::parentWidget() const
{
	return dynamic_cast<QWidget*> (m_loader->parent());
}

QWebEngineView* TmsLoader::Impl::getWebView()
{
	if (m_webViewPool.size() == 0) {
		auto newView = new QWebEngineView(parentWidget());
		// This is for hiding the QWebEngineView. When debugging, comment out the following line.
		newView->move(WINDOW_BIG_WIDTH, 0);
		newView->resize(1, 1);
		newView->show();
		return newView;
	} else {
		auto ret = m_webViewPool.back();
		m_webViewPool.pop_back();
		return ret;
	}
}

TmsLoader::TmsLoader(QWidget* parent) :
	QObject (parent),
	impl (new Impl(this))
{}

TmsLoader::~TmsLoader()
{
	delete impl;
}

void TmsLoader::registerRequest(const TmsRequest& request, int* requestId)
{
	TmsRequestHandler* handler = impl->registerNewHandler(request);
	*requestId = handler->requestId();

	connect(handler, SIGNAL(imageUpdated()), this, SLOT(handleUpdate()));
}

void TmsLoader::cancelRequest(int requestId)
{
	if (requestId >= impl->m_handlers.size()) {return;}
	TmsRequestHandler* handler = impl->m_handlers.at(requestId);
	if (handler == nullptr) {return;}

	impl->m_webViewPool.push_back(handler->webView());
	delete handler;
	impl->m_handlers[requestId] = nullptr;
}

QImage TmsLoader::getImage(int requestId) const
{
	QImage dummyImage;
	if (requestId >= impl->m_handlers.size()) {return dummyImage;}
	TmsRequestHandler* handler = impl->m_handlers.at(requestId);
	if (handler == nullptr) {return dummyImage;}

	return handler->image();
}

void TmsLoader::handleUpdate()
{
	TmsRequestHandler* handler = qobject_cast<TmsRequestHandler*> (sender());

	emit imageUpdated(handler->requestId());
}
