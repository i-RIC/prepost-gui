#ifndef TMSLOADER_TMSLOADER_IMPL_H
#define TMSLOADER_TMSLOADER_IMPL_H

#include "../tmsimagecache.h"
#include "../tmsloader.h"

#include <QPixmap>
#include <QString>

#include <unordered_map>
#include <vector>

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

	std::vector<TmsRequestHandler*> m_handlers;
	TmsImageCache m_imageCache;

	TmsLoader* m_loader;
};

} // tmsloader

#endif // TMSLOADER_TMSLOADER_IMPL_H
