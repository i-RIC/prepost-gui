#include "iricrootpath.h"
#include "ziparchive.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QString>
#include <QStringList>

namespace iRIC
{
	bool ZipArchive(const QString& zipfilename, const QString& path, const QStringList& filelist)
	{
		// if the target file already exists, remove it first.
		QFile f(zipfilename);
		if (f.exists()) {
			bool ret = f.remove(zipfilename);
			if (ret == false) {
				// failed to remove!
				return false;
			}
		}
		QProcess* process = createZipArchiveProcess(zipfilename, path, filelist);
		process->waitForFinished(-1);
		return true;
	}
	QProcess* createZipArchiveProcess(const QString& zipfilename, const QString& path, const QStringList& filelist)
	{
		QString exepath = iRICRootPath::get();
		QString zipexe = QDir(exepath).absoluteFilePath("zip.exe");
		QProcess* zipprocess = new QProcess();
		zipprocess->setWorkingDirectory(path);
		QStringList args;
		args << zipfilename << filelist;
		zipprocess->start(zipexe, args);
		return zipprocess;
	}

	void UnzipArchive(const QString& zipfilename, const QString& targetfolder)
	{
		QProcess* process = createUnzipArchiveProcess(zipfilename, targetfolder);
		process->waitForFinished(-1);
	}
	QProcess* createUnzipArchiveProcess(const QString& zipfilename, const QString& targetfolder)
	{
		QString exepath = iRICRootPath::get();
		QString unzipexe = QDir(exepath).absoluteFilePath("unzip.exe");
		QProcess* zipprocess = new QProcess();
		QStringList args;
		args << "-o" << zipfilename << "-d" << targetfolder;
		zipprocess->start(unzipexe, args);
		return zipprocess;
	}
}

