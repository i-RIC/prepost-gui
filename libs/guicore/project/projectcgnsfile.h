#ifndef PROJECTCGNSFILE_H
#define PROJECTCGNSFILE_H

#include "../guicore_global.h"

#include <QString>

#include <string>

class SolverDefinitionAbstract;
class VersionNumber;

namespace iRICLib {
	class H5CgnsFile;
} // namespase iRICLib

/// The class to define CGNS File related constants.
class GUICOREDLL_EXPORT ProjectCgnsFile
{
public:
	static int writeSolverInfo(iRICLib::H5CgnsFile* file, const SolverDefinitionAbstract& solverDef);
	static bool readSolverInfo(const QString& filename, std::string* solverName, VersionNumber* version);
	static int readSolverInfo(const iRICLib::H5CgnsFile& file, std::string* solverName, VersionNumber* version);

private:
	ProjectCgnsFile();
};

#endif // PROJECTCGNSFILE_H
