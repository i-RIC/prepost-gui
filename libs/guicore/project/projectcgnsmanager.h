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
	std::string inputFileName() const;
	std::string inputFileFullName() const;

	bool inputFileExists() const;
	bool separateResultExists() const;
	bool deleteResultFolder();
	void copyInputFileToMainFile();

	QStringList containedFiles() const;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/projectcgnsmanager_impl.h"
#endif // _DEBUG

#endif // PROJECTCGNSMANAGER_H
