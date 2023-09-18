#include "projectdata.h"
#include "workfolderinfo.h"
#include "private/workfolderinfo_impl.h"

#include <misc/stringtool.h>

#include <QDir>
#include <QLockFile>

WorkfolderInfo::Impl::Impl(const QString& workfolder) :
	m_absolutePath {workfolder}
{
	QDir wFolder(workfolder);
	m_folderName = wFolder.dirName();

	QString lockFileName = wFolder.absoluteFilePath(ProjectData::LOCKFILENAME);
	QLockFile lockFile(lockFileName);
	lockFile.setStaleLockTime(0);
	bool ok = lockFile.tryLock();

	m_isLocked = ! ok;

	lockFile.unlock();
}

WorkfolderInfo::WorkfolderInfo(const QString& workfolder, QObject* parent) :
	QObject {parent},
	impl {new Impl(workfolder)}
{}

WorkfolderInfo::~WorkfolderInfo()
{}

QString WorkfolderInfo::folderName() const
{
	return impl->m_folderName;
}

QString WorkfolderInfo::absolutePath() const
{
	return impl->m_absolutePath;
}

bool WorkfolderInfo::isLocked() const
{
	return impl->m_isLocked;
}
