#include "cgnsutil.h"
#include "../project/projectcgnsfile.h"

#include <cgnslib.h>

int CgnsUtil::arrayIdWithName(const std::string& name)
{
	int ier;
	int numArrays;

	ier = cg_narrays(&numArrays);
	for (int A = 1; A <= numArrays; ++A) {
		char n[ProjectCgnsFile::BUFFERLEN];
		DataType_t dataType;
		int dim;
		cgsize_t dimVec;

		ier = cg_array_info(A, n, &dataType, &dim, &dimVec);
		if (strcmp(name.c_str(), n) == 0) {return A;}
	}
	return 0;
}

int CgnsUtil::findArray(const QString& name, DataType_t dt, int dim, int narrays)
{
	for (int i = 1; i <= narrays; ++i) {
		char arrayname[ProjectCgnsFile::BUFFERLEN];
		DataType_t datatype;
		int dimension;
		cgsize_t dimVector[3];
		cg_array_info(i, arrayname, &datatype, &dimension, dimVector);
		if (name == arrayname && dt == datatype && dim == dimension) {
			return i;
		}
	}
	return 0;
}

int CgnsUtil::findArray(const QString& name, DataType_t* dt, int* dim, cgsize_t *dimVec, int narrays)
{
	for (int i = 1; i <= narrays; ++i) {
		char arrayname[ProjectCgnsFile::BUFFERLEN];
		cg_array_info(i, arrayname, dt, dim, dimVec);
		if (name == arrayname) {
			return i;
		}
	}
	return 0;
}

CgnsUtil::CgnsUtil()
{}
