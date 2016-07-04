#include "unziparchiveprocessexecuter.h"

#include <misc/ziparchive.h>

UnzipArchiveProcessExecuter::UnzipArchiveProcessExecuter(const QString& zipFileName, const QString& targetFolder) :
	ProcessExecuter(iRIC::createUnzipArchiveProcess(zipFileName, targetFolder))
{}
