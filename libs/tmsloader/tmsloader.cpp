#include "tmsloader.h"
#include "tmsrequest.h"
#include "private/tmsloader_impl.h"
#include "private/tmsrequesthandler.h"

#include <QPixmap>

using namespace tmsloader;

TmsLoader::Impl::Impl(TmsLoader* parent) :
	m_loader (parent)
{}

TmsLoader::Impl::~Impl()
{
	for (auto handler : m_handlers) {
		delete handler;
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

	TmsRequestHandler* handler = request.buildHandler(newId, m_loader);
	m_handlers[newId] = handler;

	return handler;
}

TmsLoader::TmsLoader(QObject* parent) :
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
