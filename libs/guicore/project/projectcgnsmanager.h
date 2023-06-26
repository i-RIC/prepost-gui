#ifndef PROJECTCGNSMANAGER_H
#define PROJECTCGNSMANAGER_H

#include "../guicore_global.h"

#include <string>

class ProjectMainFile;

class QStringList;

class GUICOREDLL_EXPORT ProjectCgnsManager
{
public:
	ProjectCgnsManager(ProjectMainFile* mainFile);
	~ProjectCgnsManager();

	std::string mainFileName() const;
	std::string mainFileFullName() const;
	std::string backupFileName() const;
	std::string backupFileFullName() const;

	bool backupFileExists() const;
	bool separateResultExists() const;
	bool deleteResultFolder();

	QStringList containedFiles() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/projectcgnsmanager_impl.h"
#endif // _DEBUG

#endif // PROJECTCGNSMANAGER_H
