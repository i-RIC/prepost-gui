#ifndef ZIPARCHIVEPROCESSEXECUTER_H
#define ZIPARCHIVEPROCESSEXECUTER_H

#include "../guibase_global.h"
#include "processexecuter.h"

class GUIBASEDLL_EXPORT ZipArchiveProcessExecuter : public ProcessExecuter
{
public:
	ZipArchiveProcessExecuter(const QString& zipFileName, const QString& path, const QStringList& filelist);
};

#endif // ZIPARCHIVEPROCESSEXECUTER_H
