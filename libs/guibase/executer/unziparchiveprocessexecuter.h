#ifndef UNZIPARCHIVEPROCESSEXECUTER_H
#define UNZIPARCHIVEPROCESSEXECUTER_H

#include "../guibase_global.h"
#include "processexecuter.h"

class GUIBASEDLL_EXPORT UnzipArchiveProcessExecuter : public ProcessExecuter
{
public:
	UnzipArchiveProcessExecuter(const QString& zipFileName, const QString& targetFolder);
};

#endif // UNZIPARCHIVEPROCESSEXECUTER_H
