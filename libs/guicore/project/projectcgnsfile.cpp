#include "projectcgnsfile.h"
#include <misc/stringtool.h>
#include <misc/versionnumber.h>
#include "../solverdef/solverdefinitionabstract.h"
#include <cgnslib.h>
#include <iriclib.h>
#include <QString>

#if CGNS_VERSION < 3100
#define cgsize_t int
#endif

bool ProjectCgnsFile::createNewFile(const QString& filename, int cell_dim, int phys_dim)
{
	int fn;
	int ret;
	// open cgns file with write mode.
	ret = cg_open(iRIC::toStr(filename).c_str(), CG_MODE_WRITE, &fn);
	if (ret != 0){goto ERROR_OPENING;}
	// create "iRIC" base.
	int base;
	ret = cg_base_write(fn, "iRIC", cell_dim, phys_dim, &base);
	if (ret != 0){goto ERROR_AFTER_OPENING;}
	// create "CalculationConditions" node just under iRIC base.
	ret = cg_gopath(fn, "/iRIC");
	if (ret != 0){goto ERROR_AFTER_OPENING;}
	ret = cg_user_data_write("CalculationConditions");
	if (ret != 0){goto ERROR_AFTER_OPENING;}
	// That's all, for initialization. so close it.
	ret = cg_close(fn);
	return (ret == 0);

ERROR_AFTER_OPENING:
	cg_close(fn);
ERROR_OPENING:
	return false;
}

bool ProjectCgnsFile::writeSolverInfo(const QString& filename, const SolverDefinitionAbstract* solverDef)
{
	int fn;
	int ret;
	bool bret;

	ret = cg_open(iRIC::toStr(filename).c_str(), CG_MODE_MODIFY, &fn);
	if (ret != 0){return false;}
	bret = writeSolverInfo(fn, solverDef);
	cg_close(fn);
	return bret;
}

bool ProjectCgnsFile::writeSolverInfo(int fn, const SolverDefinitionAbstract* solverDef)
{
	if (checkSolverInfo(fn, solverDef)){
		// check passed. it means that right solver information is already written.
		return true;
	}
	int ret;
	cgsize_t dim[3];
	QString versionString;

	QString solverName = solverDef->name();
	VersionNumber version = solverDef->version();

	// goto "iRIC" base".
	ret = cg_gopath(fn, "/iRIC");
	if (ret != 0){return false;}
	// remove "SolverInformation" node if it exists.
	// we do know check the result because it returns other than 1 when
	// the node does not exists.
	cg_delete_node("SolverInformation");
	// add new SolverInformation node
	ret = cg_user_data_write("SolverInformation");
	if (ret != 0){return false;}
	ret = cg_gopath(fn, "/iRIC/SolverInformation");
	if (ret != 0){return false;}
	// add name node
	dim[0] = solverName.length() + 1;
	ret = cg_array_write("Name", Character, 1, dim, iRIC::toStr(solverName).c_str());
	if (ret != 0){return false;}
	// add version node
	versionString = version.toString();
	dim[0] = versionString.length() + 1;
	ret = cg_array_write("Version", Character, 1, dim, iRIC::toStr(versionString).c_str());
	if (ret != 0){return false;}
	return true;
}

bool ProjectCgnsFile::checkSolverInfo(int fn, const SolverDefinitionAbstract* solverDef)
{
	int ret;
	int dim;
	cgsize_t dimVec[3];
	DataType_t dataType;
	char arrayName[BUFFERLEN];
	QString solverName = solverDef->name();
	VersionNumber version = solverDef->version();
	VersionNumber v;
	char* buffer = 0;

	// goto "iRIC/SolverInformation" base".
	ret = cg_gopath(fn, "/iRIC/SolverInformation");
	if (ret != 0){return false;}
	// read name node
	ret = cg_array_info(1, arrayName, &dataType, &dim, dimVec);
	if (ret != 0){return false;}
	if (QString(arrayName) != "Name"){return false;}
	buffer = new char[dimVec[0] + 1];
	ret = cg_array_read(1, buffer);
	if (ret != 0){goto ERROR_AFTER_BUFFER_ALLOC;}
	if (solverName != buffer){
		goto ERROR_AFTER_BUFFER_ALLOC;
	}
	delete buffer;

	// the second one is "Version" array.
	ret = cg_array_info(2, arrayName, &dataType, &dim, dimVec);
	if (ret != 0){return false;}
	if (QString(arrayName) != "Version"){return false;}
	buffer = new char[dimVec[0] + 1];
	ret = cg_array_read(2, buffer);
	if (ret != 0){goto ERROR_AFTER_BUFFER_ALLOC;}
	v.fromString(buffer);
	delete buffer;
	if (version != v){
		return false;
	}
	return true;

ERROR_AFTER_BUFFER_ALLOC:
	delete buffer;
	return false;
}


bool ProjectCgnsFile::readSolverInfo(const QString& filename, QString& solverName, VersionNumber& version)
{
	int fn;
	int ret;

	ret = cg_open(iRIC::toStr(filename).c_str(), CG_MODE_READ, &fn);
	if (ret != 0){goto ERROR_OPENING;}

	if (! readSolverInfo(fn, solverName, version)){
		goto ERROR_AFTER_OPENING;
	}

	ret = cg_close(fn);
	return (ret == 0);

ERROR_AFTER_OPENING:
	cg_close(fn);
ERROR_OPENING:
	return false;
}

bool ProjectCgnsFile::readSolverInfo(int fn, QString& solverName, VersionNumber& version)
{
	int ret;
	int dim;
	cgsize_t dimVec[3];
	char arrayName[BUFFERLEN];
	DataType_t dataType;

	// goto "iRIC/SolverInformation"
	ret = cg_gopath(fn, "/iRIC/SolverInformation");
	if (ret != 0){return false;}
	// the first one is "Name" array.
	ret = cg_array_info(1, arrayName, &dataType, &dim, dimVec);
	if (ret != 0){return false;}
	if (QString(arrayName) != "Name"){return false;}
	char* buffer = new char[dimVec[0] + 1];
	ret = cg_array_read(1, buffer);
	if (ret != 0){return false;}
	solverName = buffer;
	delete buffer;

	// the second one is "Version" array.
	ret = cg_array_info(2, arrayName, &dataType, &dim, dimVec);
	if (ret != 0){return false;}
	if (QString(arrayName) != "Version"){return false;}
	buffer = new char[dimVec[0] + 1];
	ret = cg_array_read(2, buffer);
	if (ret != 0){return false;}
	version.fromString(buffer);
	delete buffer;

	return true;
}

const QString ProjectCgnsFile::acceptablePattern()
{
	return "^[a-zA-Z0-9_\\-]+$";
}
