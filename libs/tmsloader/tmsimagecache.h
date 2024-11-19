#ifndef TMSIMAGECACHE_H
#define TMSIMAGECACHE_H

#include <QDir>
#include <QTemporaryDir>

class QPixmap;
class QString;

class TmsImageCache
{
public:
	TmsImageCache();
	~TmsImageCache();

	bool exists(const QString& url) const;
	QPixmap load(const QString& url) const;
	void save(const QString& url, const QPixmap& image);

private:
	QString fileName(const QString& url) const;

	QTemporaryDir m_tempDir;
	QDir m_dir;
};

#endif // TMSIMAGECACHE_H
