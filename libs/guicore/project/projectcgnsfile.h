#ifndef PROJECTCGNSFILE_H
#define PROJECTCGNSFILE_H

#include "../guicore_global.h"

#include <QString>

class SolverDefinitionAbstract;
class VersionNumber;

/// The class to define CGNS File related constants.
class GUICOREDLL_EXPORT ProjectCgnsFile
{
public:
	const static int BUFFERLEN = 200;
	static bool createNewFile(const QString& filename, int cell_dim, int phys_dim);
	static bool writeSolverInfo(const QString& filename, const SolverDefinitionAbstract* solverDef);
	static bool writeSolverInfo(int fn, const SolverDefinitionAbstract* solverDef);
	static bool readSolverInfo(const QString& filename, QString& solverName, VersionNumber& version);
	static bool readSolverInfo(int fn, QString& solverName, VersionNumber& version);
	static const QString acceptablePattern();
private:
	static bool checkSolverInfo(int fn, const SolverDefinitionAbstract* solverDef);
	ProjectCgnsFile();
};

#endif // PROJECTCGNSFILE_H
