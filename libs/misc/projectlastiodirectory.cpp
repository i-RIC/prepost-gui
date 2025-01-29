#include "projectlastiodirectory.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>

QString ProjectLastIODirectory::get()
{
	QSettings settings;
	auto dir = settings.value("general/projectlastiodir", QDir::homePath()).toString();
	if (dir == "" || ! QDir(dir).exists()) {
		dir = QDir::homePath();
	}
	return dir;
}

void ProjectLastIODirectory::set(const QString& val)
{
	QSettings settings;
	settings.setValue("general/projectlastiodir", val);
}

void ProjectLastIODirectory::setFromFilename(const QString& filename)
{
	QFileInfo finfo(filename);
	set(finfo.absolutePath());
}

ProjectLastIODirectory::ProjectLastIODirectory()
{}
