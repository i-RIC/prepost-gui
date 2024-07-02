#include "folderbackup.h"
#include "folderremover.h"

#include <QDir>

FolderBackup::FolderBackup(const QString& folderName, const QString& backupFolder):
	m_folderName {folderName},
	m_backupFolder {backupFolder},
	m_remover {nullptr}
{
	QDir dir(folderName);
	m_remover.reset(new FolderRemover(dir.absoluteFilePath(backupFolder)));

	backup();
}

FolderBackup::~FolderBackup()
{}

void FolderBackup::backup()
{
	QDir dir(m_folderName);
	if (! dir.exists()) {return;}

	dir.mkdir(m_backupFolder);
	QDir backupDir(dir.absoluteFilePath(m_backupFolder));

	for (auto fileName : dir.entryList(QDir::Files, QDir::NoSort)) {
		auto srcFile = dir.absoluteFilePath(fileName);
		auto tgtFile = backupDir.absoluteFilePath(fileName);
		QFile f(srcFile);
		f.copy(tgtFile);
	}
}

void FolderBackup::restore()
{
	QDir dir(m_folderName);
	QDir backupDir(dir.absoluteFilePath(m_backupFolder));

	for (auto fileName : backupDir.entryList(QDir::Files, QDir::NoSort)) {
		auto srcFile = backupDir.absoluteFilePath(fileName);
		auto tgtFile = dir.absoluteFilePath(fileName);
		QFile f(srcFile);
		f.copy(tgtFile);
	}
}
