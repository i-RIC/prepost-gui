#ifndef TMSLOADER_TMSLOADER_IMPL_H
#define TMSLOADER_TMSLOADER_IMPL_H

#include "../tmsloader.h"

#include <QNetworkProxy>
#include <QNetworkAccessManager>

#include <vector>

namespace tmsloader {

class TmsRequestHandler;

class TmsLoader::Impl
{
public:
	Impl(TmsLoader* parent);
	~Impl();

	TmsRequestHandler* registerNewHandler(const TmsRequest& request);

	std::vector<TmsRequestHandler*> m_handlers;
	TmsLoader* m_loader;
	QNetworkAccessManager m_accessManager;
};

} // tmsloader

#endif // TMSLOADER_TMSLOADER_IMPL_H
