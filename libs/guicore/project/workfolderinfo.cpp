#include "projectdata.h"
#include "workfolderinfo.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

WorkfolderInfo::WorkfolderInfo(const QString& workfolder, QObject* parent)
	: QObject(parent)
{
	m_absolutePath = workfolder;
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
