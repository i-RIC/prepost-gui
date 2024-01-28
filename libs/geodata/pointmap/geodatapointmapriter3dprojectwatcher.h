#ifndef GEODATAPOINTMAPRITER3DPROJECTWATCHER_H
#define GEODATAPOINTMAPRITER3DPROJECTWATCHER_H

#include <QObject>

class GeoDataPointmap;

class QFileSystemWatcher;

class GeoDataPointmapRiter3dProjectWatcher : public QObject
{
	Q_OBJECT

public:
	GeoDataPointmapRiter3dProjectWatcher(const QString& fileName, GeoDataPointmap* pointmap);

private slots:
	void load();

private:
	QString m_fileName;
	GeoDataPointmap* m_pointmap;

	QFileSystemWatcher* m_watcher;
};

#endif // GEODATAPOINTMAPRITER3DPROJECTWATCHER_H
