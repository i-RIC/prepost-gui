#ifndef WORKFOLDERINFO_IMPL_H
#define WORKFOLDERINFO_IMPL_H

#include "../workfolderinfo.h"

#include <QString>
#include <QDateTime>

class WorkfolderInfo::Impl
{
public:
	Impl(const QString& workfolder);

	QString m_folderName;
	QDateTime m_lastModifiedTime;
	QString m_absolutePath;
	bool m_isLocked;
};

#endif // WORKFOLDERINFO_IMPL_H
