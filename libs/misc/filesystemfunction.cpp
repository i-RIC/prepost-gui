#include "filesystemfunction.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTime>
#include <QByteArray>
#include <QString>
#include <QCryptographicHash>


bool iRIC::mkdirRecursively(const QString& dirname)
{
	QDir dir(dirname);
	if (dir.exists()) {return true;}

	QStringList frags = dirname.split('/');
	QString lastDir = frags.back();
	frags.pop_back();
	QString parentDir = frags.join('/');
	bool ok = mkdirRecursively(parentDir);
	if (! ok) {return false;}
	QDir pdir(parentDir);
	return pdir.mkdir(lastDir);
}

bool iRIC::rmdirRecursively(const QString& dirname)
{
	QDir dir(dirname);
	/// remove subdirectories
	QStringList subdirs = dir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
	for (int i = 0; i < subdirs.size(); ++i) {
		QString subdir = dir.absoluteFilePath(subdirs.at(i));
		bool ok = iRIC::rmdirRecursively(subdir);
		if (! ok) { return false; }
	}
	// remove files
	QStringList files = dir.entryList(QStringList(), QDir::Files, QDir::Name);
	for (int i = 0; i < files.size(); ++i) {
		bool ok = dir.remove(files.at(i));
		if (! ok) { return false; }
	}
	// remove this folder
	QDir parent = dir;
	parent.cdUp();
	bool ok = parent.rmdir(dir.dirName());
	return ok;
}

bool iRIC::copyDirRecursively(const QString& fromname, const QString& toname, int depth)
{
	if (depth == 0) {return true;}
	// create target folder if it does not exists yet.
	QFileInfo toDirInfo(toname);
	bool ok = false;
	if (toDirInfo.exists()) {
		if (toDirInfo.isDir()) {
			// it alredy exists
			ok = true;
		} else {
			// it is not a directory. Maybe file. remove it.
			bool removeok = QFile::remove(toname);
			if (! removeok) {return false;}
		}
	}
	if (! ok) {
		// the folder does not exists now.
		QFileInfo toInfo(toname);
		QDir dir = toInfo.absoluteDir();
		QString dirName = toInfo.fileName();
		ok = dir.mkdir(dirName);
	}
	if (! ok) {
		// mkdir failed.
		return false;
	}
	QDir fromDir = QDir(fromname);
	QDir toDir = QDir(toname);
	// Copy sub directories
	QStringList subdirs = fromDir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
	for (int i = 0; i < subdirs.size(); ++i) {
		QString fromSubdir = fromDir.absoluteFilePath(subdirs.at(i));
		QString toSubdir = toDir.absoluteFilePath(subdirs.at(i));
		ok = iRIC::copyDirRecursively(fromSubdir, toSubdir, depth - 1);
		if (! ok) {return false;}
	}
	// Copy files
	QStringList files = fromDir.entryList(QStringList(), QDir::Files, QDir::Name);
	for (int i = 0; i < files.size(); ++i) {
		QString fromFile = fromDir.absoluteFilePath(files.at(i));
		QString toFile = toDir.absoluteFilePath(files.at(i));
		bool ok = QFile::copy(fromFile, toFile);
		if (! ok) {return false;}
	}
	return true;
}

bool iRIC::moveDirContent(const QString& fromname, const QString& toname)
{
	// create target folder if it does not exists yet.
	QFileInfo toDirInfo(toname);
	bool ok = false;
	if (! toDirInfo.exists() || ! toDirInfo.isDir()) {
		return false;
	}
	QDir fromDir = QDir(fromname);
	QDir toDir = QDir(toname);
	// Copy sub directories
	QStringList subitems = fromDir.entryList(QStringList(), QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
	for (int i = 0; i < subitems.size(); ++i) {
		QString fromSubitem = fromDir.absoluteFilePath(subitems.at(i));
		QString toSubitem = toDir.absoluteFilePath(subitems.at(i));
		ok = QFile::rename(fromSubitem, toSubitem);
		if (! ok) {return false;}
	}
	return true;
}

bool iRIC::isDirEmpty(const QString& dirname)
{
	QDir dir = QDir(dirname);
	// Copy sub directories
	QStringList subdirs = dir.entryList(QStringList(), QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
	int count = subdirs.count();
	return count == 0;
}

QString iRIC::getTempFileName(const QString folder)
{
	QDir folderDir(folder);

	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData("iRIC", 4);
	QTime current = QTime::currentTime();
	qsrand(current.msec());
	hash.addData(QByteArray(1, qrand()));

	while (folderDir.exists(hash.result().toHex())) {
		hash.addData(QByteArray(1, qrand()));
	}
	return folderDir.absoluteFilePath(hash.result().toHex());
}

