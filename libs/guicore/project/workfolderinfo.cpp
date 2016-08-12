#include "projectdata.h"
#include "workfolderinfo.h"
#include "private/workfolderinfo_impl.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

WorkfolderInfo::Impl::Impl(const QString& workfolder) :
	m_absolutePath {workfolder}
{
	QDir wFolder(workfolder);
	QFile lockFile(wFolder.absoluteFilePath(ProjectData::LOCKFILENAME));
	m_folderName = wFolder.dirName();

	if (lockFile.exists()) {
		// if lock file exists, use lastmodified time of lock file
		QFileInfo info(lockFile);
		m_lastModifiedTime = info.lastModified();
		m_isLocked = (! info.isWritable());
	} else {
		// if lock file doesn't exists, use lastmodified time of workfolder
		QFileInfo info(workfolder);
		m_lastModifiedTime = info.lastModified();
		m_isLocked = false;
	}
}

WorkfolderInfo::WorkfolderInfo(const QString& workfolder, QObject* parent) :
	QObject {parent},
	impl {new Impl(workfolder)}
{}

WorkfolderInfo::~WorkfolderInfo()
{
	delete impl;
}

QString WorkfolderInfo::folderName() const
{
	return impl->m_folderName;
}

QDateTime WorkfolderInfo::lastModifiedTime() const
{
	return impl->m_lastModifiedTime;
}

QString WorkfolderInfo::absolutePath() const
{
	return impl->m_absolutePath;
}

bool WorkfolderInfo::isLocked() const
{
	return impl->m_isLocked;
}
