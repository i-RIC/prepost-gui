#include "ziparchiveprocessexecuter.h"

#include <misc/ziparchive.h>

ZipArchiveProcessExecuter::ZipArchiveProcessExecuter(const QString& zipFileName, const QString& path, const QStringList& filelist) :
	ProcessExecuter(iRIC::createZipArchiveProcess(zipFileName, path, filelist))
{}
