#ifndef IRICLIB_CGNSFILE_IMPL_H
#define IRICLIB_CGNSFILE_IMPL_H

#include "../iriclib_cgnsfile.h"
#include "iriclib_cgnsfile_baseiterativet.h"

#include <string>
#include <vector>

namespace iRICLib {

class CgnsFile::Impl
{
public:
	int initBaseId(bool clearResults, char* baseName = NULL);
	int initZoneId(bool clearResults);

	int clearResultData();
	int clearBaseIter(); // local_init_biter_Mul
	int clearZoneIter(); // local_init_ziter_Mul
	int clearSolution();
	int clearParticleSolution();
	int clearResultGrids();

	int loadResultData(); // local_load_sol_Mul
	int loadBcNames();

	int gotoBase();
	int gotoBaseIter();

	int gotoCCBase();
	int gotoCCBaseChild(const char* path); // local_gotoccb_Mul
	int gotoCC(); // local_gotocc_Mul
	int gotoCCChild(const char* path); // local_gotoccchild_Mul
	int gotoGeoData(); // local_gotord_Mul

	int gotoCCBaseIter();

	int gotoZone();
	int gotoZoneChild(const char* path); // local_gotoz_Mul
	int gotoGridCondition(); // local_gotogc_Mul
	int gotoGridConditionChild(const char* path); // local_gotogcchild_Mul
	int gotoGridConditionNewChild(const char* path); // local_gotogcnewchild_Mul

	int gotoZoneIter();

	int gotoComplexGroup(const char* groupName); // local_gotocomplexgroup_Mul
	int gotoComplex(const char* groupName, int num); // local_gotocomplex_Mul
	int gotoComplexChild(const char* groupName, int num, const char* name); //local_gotocomplexchild_Mul
	int gotoComplexNewChild(const char* groupName, int num, const char* name); // local_gotocomplexchild_create_Mul
	int addComplexNodeIfNotExist(); // local_complex_add_gccnode_Mul

	static int findArray(const char* name, int* index, DataType_t* dt, int* dim, cgsize_t* dimVec); //local_find_array
	static int readArray(const char* name, DataType_t dt, cgsize_t len, void* memory); // local_read_array
	static int readArrayAs(const char* name, DataType_t dt, size_t length, void* memory); // local_read_array_as
	static int readStringLen(const char* name, int* length); // local_read_string_len
	static int readString(const char* name, size_t bufferLen, char* buffer); // local_read_string

	static int writeArray(const char* name, DataType_t dt, size_t length, void* memory); //local_write_array
	static int writeString(const char* name, char* value); // local_write_string

	static void getComplexName(int num, char *name); // local_get_complex_name

	int m_fileId = 0;

	int m_baseId = 0;
	int m_ccBaseId = 0;
	int m_baseCellDim = 0;
	int m_basePhysDim = 0;

	int m_zoneId;
	cgsize_t m_zoneSize[9];

	std::vector<std::string> m_bcNames;

	int m_solId;
	std::vector<double> m_solTimes;
	std::vector<double> m_solIndices;
	std::vector<std::string> m_solGridCoordPointers;
	std::vector<std::string> m_solPointers;
	std::vector<std::string> m_solParticlePointers;

	std::vector<BaseIterativeT<int> > m_solBaseIterInts;
	std::vector<BaseIterativeT<double> > m_solBaseIterReals;
};

} // iRICLib

#endif // IRICLIB_CGNSFILE_IMPL_H
