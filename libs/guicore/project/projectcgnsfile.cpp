#include "../solverdef/solverdefinitionabstract.h"
#include "projectcgnsfile.h"

#include <misc/stringtool.h>
#include <misc/versionnumber.h>

#include <QString>

#include <h5cgnsfile.h>
#include <h5cgnsbase.h>
#include <h5cgnssolverinformation.h>
#include <iriclib_errorcodes.h>

#include <vector>

int ProjectCgnsFile::writeSolverInfo(iRICLib::H5CgnsFile* file, const SolverDefinitionAbstract& solverDef)
{
	auto info = file->ccBase()->solverInformation();

	int ier = info->writeSolverName(solverDef.name());
	if (ier != IRIC_NO_ERROR) {return ier;}

	ier = info->writeSolverVersion(iRIC::toStr(solverDef.version().toString()));
	if (ier != IRIC_NO_ERROR) {return ier;}

	return IRIC_NO_ERROR;
}

bool ProjectCgnsFile::readSolverInfo(const QString& filename, std::string* solverName, VersionNumber* version)
{
	try {
		iRICLib::H5CgnsFile file(iRIC::toStr(filename), iRICLib::H5CgnsFile::Mode::OpenReadOnly);
		return readSolverInfo(file, solverName, version);
	} catch (...) {
		return false;
	}
}

int ProjectCgnsFile::readSolverInfo(const iRICLib::H5CgnsFile& file, std::string* solverName, VersionNumber* version)
{
	auto info = file.ccBase()->solverInformation();

	int ier = info->readSolverName(solverName);
	if (ier != IRIC_NO_ERROR) {return ier;}

	std::string versionStr;
	ier = info->readSolverVersion(&versionStr);
	if (ier != IRIC_NO_ERROR) {return ier;}
	*version = VersionNumber(versionStr.c_str());

	return IRIC_NO_ERROR;
}
