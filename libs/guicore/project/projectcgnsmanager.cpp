#include "projectcgnsmanager.h"
#include "projectmainfile.h"
#include "private/projectcgnsmanager_impl.h"

#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>

#include <QDir>

#include <h5cgnsfileseparatesolutionutil.h>

#include <sstream>

namespace {

const QString MAIN_FILENAME = "Case1.cgn";
const QString BACKUP_FILENAME = "Case1_input.cgn";

} // namespace

ProjectCgnsManager::ProjectCgnsManager(ProjectMainFile* mainFile) :
	impl {new Impl {mainFile}}
{}

ProjectCgnsManager::~ProjectCgnsManager()
{
	delete impl;
}

std::string ProjectCgnsManager::mainFileName() const
{
	return iRIC::toStr(MAIN_FILENAME);
}

std::string ProjectCgnsManager::mainFileFullName() const
{
	QDir dir(impl->m_mainFile->workDirectory());
	return iRIC::toStr(dir.absoluteFilePath(MAIN_FILENAME));
}

std::string ProjectCgnsManager::backupFileName() const
{
	return iRIC::toStr(BACKUP_FILENAME);
}

std::string ProjectCgnsManager::backupFileFullName() const
{
	QDir dir(impl->m_mainFile->workDirectory());
	return iRIC::toStr(dir.absoluteFilePath(BACKUP_FILENAME));
}

bool ProjectCgnsManager::backupFileExists() const
{
	QFile file(backupFileFullName().c_str());
	return file.exists();
}

bool ProjectCgnsManager::separateResultExists() const
{
	QDir dir(impl->m_mainFile->workDirectory());
	QString resultFolder = dir.absoluteFilePath("result");
	QDir resultDir(resultFolder);
	if (! resultDir.exists()) {return false;}

	// Check if Solution1.cgn exists
	auto fname = iRICLib::H5CgnsFileSeparateSolutionUtil::fileNameForSolution(iRIC::toStr(resultFolder), 1);
	QFile file(fname.c_str());
	return file.exists();
}

bool ProjectCgnsManager::deleteResultFolder()
{
	QDir dir(impl->m_mainFile->workDirectory());
	QString resultFolder = dir.absoluteFilePath("result");
	QDir resultDir(resultFolder);
	if (! resultDir.exists()) {return true;}

	return iRIC::rmdirRecursively(resultFolder);
}

QStringList ProjectCgnsManager::containedFiles() const
{
	QStringList ret;
	ret << MAIN_FILENAME;

	QDir resultDir(impl->m_mainFile->workDirectory());
	bool ok = resultDir.cd("result");
	if (! ok) {return ret;}

	QStringList files = resultDir.entryList(QStringList(), QDir::Files);
	QDir rDir("result");
	for (const QString& file : files) {
		ret << rDir.filePath(file);
	}

	return ret;
}
