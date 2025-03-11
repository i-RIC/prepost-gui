#include "geolastiodirectory.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>

QString GeoLastIODirectory::get()
{
	QSettings settings;
	auto dir = settings.value("general/geolastiodir", QDir::homePath()).toString();
	if (dir == "" || ! QDir(dir).exists()) {
		dir = QDir::homePath();
	}
	return dir;
}

void GeoLastIODirectory::set(const QString& val)
{
	QSettings settings;
	settings.setValue("general/geolastiodir", val);
}

void GeoLastIODirectory::setFromFilename(const QString& filename)
{
	QFileInfo finfo(filename);
	set(finfo.absolutePath());
}

GeoLastIODirectory::GeoLastIODirectory()
{}
