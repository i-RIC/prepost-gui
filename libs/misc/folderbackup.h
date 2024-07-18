#ifndef FOLDERBACKUP_H
#define FOLDERBACKUP_H

#include "misc_global.h"

#include <QString>

#include <memory>

class FolderRemover;

class MISCDLL_EXPORT FolderBackup
{
public:
	FolderBackup(const QString& folderName, const QString& backupFolder = "backup");
	~FolderBackup();

	void backup();
	void restore();

private:
	QString m_folderName;
	QString m_backupFolder;

	std::unique_ptr<FolderRemover> m_remover;
};

#endif // FOLDERBACKUP_H
