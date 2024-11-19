#include "tmsimagecache.h"

#include <misc/filesystemfunction.h>

#include <QUrl>

TmsImageCache::TmsImageCache() :
	m_tempDir {},
	m_dir {m_tempDir.path()}
{}

TmsImageCache::~TmsImageCache()
{}

bool TmsImageCache::exists(const QString& url) const
{
	return QFile::exists(fileName(url));
}

QPixmap TmsImageCache::load(const QString& url) const
{
	if (! QFile::exists(fileName(url))) {
		// return null image;
		QPixmap nullImage;
		return nullImage;
	}

	QPixmap image;
	image.load(fileName(url), "png");
	return image;
}

void TmsImageCache::save(const QString& url, const QPixmap& image)
{
	auto path = fileName(url);
	bool ok = image.save(path, "png");
}

QString TmsImageCache::fileName(const QString& url) const
{
	QString localPath = url;
	localPath.replace(':', '_');
	localPath.replace('/', '_');
	localPath.replace('&', '_');

	return m_dir.absoluteFilePath(localPath);
}
