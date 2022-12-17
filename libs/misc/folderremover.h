#ifndef FOLDERREMOVER_H
#define FOLDERREMOVER_H

#include "misc_global.h"

#include <QString>

class MISCDLL_EXPORT FolderRemover
{
public:
	FolderRemover(const QString& folderName);
	~FolderRemover();

private:
	QString m_folderName;
};

#endif // FOLDERREMOVER_H
