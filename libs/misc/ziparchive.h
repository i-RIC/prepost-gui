#ifndef ZIPARCHIVE_H
#define ZIPARCHIVE_H

#include "misc_global.h"

class QString;
class QStringList;
class QProcess;

namespace iRIC
{
	/// Create a new zip file from list of files.
	bool MISCDLL_EXPORT ZipArchive(const QString& zipfilename, const QString& path, const QStringList& filelist);
	/// Extract a zip archive file.
	void MISCDLL_EXPORT UnzipArchive(const QString& zipfilename, const QString& targetfoler);
	/// Create a new process that creates a new zip file from list of files
	QProcess MISCDLL_EXPORT * createZipArchiveProcess(const QString& zipfilename, const QString& path, const QStringList& filelist);
	/// Creates a new process that extract a zip archive file.
	QProcess MISCDLL_EXPORT * createUnzipArchiveProcess(const QString& zipfilename, const QString& targetFolder);
}
#endif // ZIPARCHIVE_H
