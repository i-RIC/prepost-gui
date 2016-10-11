#include "iriclib_cgnsfile.h"
#include "private/iriclib_cgnsfile_impl.h"

#include <string>

#define RETURN_IF_ERR if (ier != 0) {return ier;}

using namespace iRICLib;

namespace {

static const int ZONESIZE_LENGTH = 9;
static const int FILEID_MAX = 100;
static const int ARRAYINCREMENTSTEP = 3;

static const std::string IRICBASE = "iRIC";
static const std::string BINAME = "BaseIterativeData";
static const std::string ZINAME = "ZoneIterativeData";

static const std::string CCNODE = "CalculationConditions";
static const std::string RDNODE = "GeographicData";
static const std::string GCNODE = "GridConditions";
static const std::string GCCNODE = "GridComplexConditions";
static const std::string ECNODE = "ErrorCode";

} // namespace

const std::string CgnsFile::Impl::IRICZONE = "iRICZone";


int CgnsFile::Impl::initBaseId(bool clearResults, char* bname)
{
	std::string baseName = IRICBASE;
	if (bname != NULL) {
		baseName = bname;
	}

	int ier;
	int nbases;
	char name[NAME_MAXLENGTH];

	ier = cg_nbases(m_fileId, &nbases);
	RETURN_IF_ERR;

	for (int B = 1; B <= nbases; ++B) {
		ier = cg_base_read(m_fileId, B, name, &m_baseCellDim, &m_basePhysDim);
		RETURN_IF_ERR;
		if (baseName == name) {
			// base found!
			m_baseId = B;
			m_ccBaseId = B;
			return initZoneId(clearResults);
		}
	}
	// not found
	return 1;
}

int CgnsFile::Impl::initZoneId(bool clearResults)
{
	int ier;
	int nzones;
	char name[NAME_MAXLENGTH];

	ier = cg_nzones(m_fileId, m_baseId, &nzones);
	RETURN_IF_ERR;

	for (int Z = 1; Z <= nzones; ++Z) {
		ier = cg_zone_read(m_fileId, m_baseId, Z, name, m_zoneSize);
		RETURN_IF_ERR;
		if (IRICZONE == name) {
			// zone found!
			m_zoneId = Z;

			if (clearResults) {
				ier = clearResultData();
			} else {
				ier = loadResultData();
			}
			RETURN_IF_ERR;

			return loadBcNames();
		}
	}
	return 1;
}

int CgnsFile::Impl::clearResultData()
{
	int ier;

	ier = clearBaseIter();
	RETURN_IF_ERR;

	ier = clearZoneIter();
	RETURN_IF_ERR;

	ier = clearSolution();
	RETURN_IF_ERR;

	ier = clearParticleSolution();
	RETURN_IF_ERR;

	ier = clearResultGrids();
	RETURN_IF_ERR;

	m_solId = 0;
	return 0;
}

int CgnsFile::Impl::clearBaseIter()
{
	int ier = gotoBase();
	RETURN_IF_ERR;
	cg_delete_node(BINAME.c_str());
	return cg_biter_write(m_fileId, m_baseId, BINAME.c_str(), 1);
}

int CgnsFile::Impl::clearZoneIter()
{
	int ier = gotoZone();
	RETURN_IF_ERR;
	cg_delete_node(ZINAME.c_str());
	return cg_ziter_write(m_fileId, m_baseId, m_zoneId, ZINAME.c_str());
}

int CgnsFile::Impl::clearSolution()
{
	int nsols;
	int ier = cg_nsols(m_fileId, m_baseId, m_zoneId, &nsols);
	RETURN_IF_ERR;

	for (int i = 0; i < nsols; ++i) {
		char name[NAME_MAXLENGTH];
		GridLocation_t location;

		ier = cg_sol_info(m_fileId, m_baseId, m_zoneId, 1, name, &location);
		RETURN_IF_ERR;
		ier = cg_delete_node(name);
		RETURN_IF_ERR;
	}
	return 0;
}

int CgnsFile::Impl::clearParticleSolution()
{
	int ier = gotoZone();
	RETURN_IF_ERR;

	int nusers;
	ier = cg_nuser_data(&nusers);
	RETURN_IF_ERR;

	for (int u = nusers; u >= 1; --u){
		char name[NAME_MAXLENGTH];
		ier = cg_user_data_read(u, name);
		if (strncmp(name, "ParticleSolution", 16) == 0) {
			ier = cg_delete_node(name);
			RETURN_IF_ERR;
		}
	}
	return 0;
}

int CgnsFile::Impl::clearResultGrids()
{
	int ngrids;
	int ier = cg_ngrids(m_fileId, m_baseId, m_zoneId, &ngrids);
	RETURN_IF_ERR;

	for (int i = 0; i < ngrids - 1; ++i) {
		char name[NAME_MAXLENGTH];
		ier = cg_grid_read(m_fileId, m_baseId, m_zoneId, 2, name);
		RETURN_IF_ERR;
		ier = cg_delete_node(name);
		RETURN_IF_ERR;
	}
	return 0;
}

int CgnsFile::Impl::loadResultData()
{
	int ier = gotoBase();
	RETURN_IF_ERR;

	ier = cg_nsols(m_fileId, m_baseId, m_zoneId, &m_solId);
	RETURN_IF_ERR;

	ier = gotoBaseIter();
	RETURN_IF_ERR;

	int narrays;
	ier = cg_narrays(&narrays);
	RETURN_IF_ERR;

	for (int i = 1; i <= narrays; ++i) {
		char name[NAME_MAXLENGTH];
		DataType_t datatype;
		int dim;
		cgsize_t dimvec;

		ier = cg_array_info(i, name, &datatype, &dim, &dimvec);
		RETURN_IF_ERR;

		if (strcmp(name, "TimeValues") == 0) {
			m_solTimes.assign(dimvec, 0);
			ier = cg_array_read(i, m_solTimes.data());
			RETURN_IF_ERR;
		} else if (strcmp(name, "IterationValues") == 0) {
			m_solIndices.assign(dimvec, 0);
			ier = cg_array_read(i, m_solIndices.data());
		} else if (datatype == RealDouble || datatype == RealSingle) {
			BaseIterativeT<double> biData = std::string(name);
			std::vector<double> vals;
			vals.assign(dimvec, 0);
			ier = cg_array_read_as(i, RealDouble, vals.data());
			RETURN_IF_ERR;
			biData.setValues(vals);
			m_solBaseIterReals.push_back(biData);
		} else if (datatype == Integer) {
			BaseIterativeT<int> biData = std::string(name);
			std::vector<int> vals;
			vals.assign(dimvec, 0);
			ier = cg_array_read_as(i, Integer, vals.data());
			RETURN_IF_ERR;
			biData.setValues(vals);
			m_solBaseIterInts.push_back(biData);
		}
	}
	return 0;
}

int CgnsFile::Impl::loadBcNames()
{
	int nbocos;
	int ier = cg_nbocos(m_fileId, m_baseId, m_zoneId, &nbocos);
	RETURN_IF_ERR;

	for (int BC = 1; BC <= nbocos; ++BC) {
		char name[NAME_MAXLENGTH];
		BCType_t bocoType;
		PointSetType_t psType;
		cgsize_t npnts;
		int normalIndex;
		cgsize_t normalListFlag;
		DataType_t normalDataType;
		int ndataset;
		ier = cg_boco_info(m_fileId, m_baseId, m_zoneId, BC, name, &bocoType, &psType,
								 &npnts, &normalIndex, &normalListFlag, &normalDataType, &ndataset);
		RETURN_IF_ERR;

		m_bcNames.push_back(std::string(name));
	}
	return 0;
}

int CgnsFile::Impl::gotoBase()
{
	return cg_goto(m_fileId, m_baseId, NULL);
}

int CgnsFile::Impl::gotoBaseIter()
{
	return cg_goto(m_fileId, m_baseId, "BaseIterativeData_t", 1, NULL);
}

int CgnsFile::Impl::gotoCCBase()
{
	return cg_goto(m_fileId, m_ccBaseId, NULL);
}

int CgnsFile::Impl::gotoCCBaseChild(const char* path)
{
	int ier = gotoCCBase();
	RETURN_IF_ERR;
	return cg_gopath(m_fileId, path);
}

int CgnsFile::Impl::gotoCC()
{
	return gotoCCBaseChild(CCNODE.c_str());
}

int CgnsFile::Impl::gotoCCChild(const char* path)
{
	int ier = gotoCC();
	RETURN_IF_ERR;
	return cg_gopath(m_fileId, path);
}

int CgnsFile::Impl::gotoCCChildCreateIfNotExist(const char* path)
{
	int ier = gotoCCChild(path);
	if (ier == 0) {return 0;}

	ier = gotoCC();
	RETURN_IF_ERR;
	ier = cg_user_data_write(path);
	RETURN_IF_ERR;
	return gotoCCChild(path);
}

int CgnsFile::Impl::gotoGeoData()
{
	return gotoCCBaseChild(RDNODE.c_str());
}

int CgnsFile::Impl::gotoCCBaseIter()
{
	return cg_goto(m_fileId, m_ccBaseId, "BaseIterativeData_t", 1, NULL);
}

int CgnsFile::Impl::gotoZone()
{
	return cg_goto(m_fileId, m_baseId, "Zone_t", m_zoneId, NULL);
}

int CgnsFile::Impl::gotoZoneChild(const char* path)
{
	int ier = gotoZone();
	RETURN_IF_ERR;
	return cg_gopath(m_fileId, path);
}

int CgnsFile::Impl::gotoGridCondition()
{
	return gotoZoneChild(GCNODE.c_str());
}

int CgnsFile::Impl::gotoGridConditionChild(const char* path)
{
	int ier = gotoGridCondition();
	RETURN_IF_ERR;
	return cg_gopath(m_fileId, path);
}

int CgnsFile::Impl::gotoGridConditionNewChild(const char* path)
{
	int ier = gotoGridCondition();
	RETURN_IF_ERR;
	// delete if exists. maybe fail, so do not result.
	cg_delete_node(path);
	// add new node
	ier = cg_user_data_write(path);
	RETURN_IF_ERR;
	return gotoGridConditionChild(path);
}

int CgnsFile::Impl::addGridConditionNodeIfNotExist()
{
	int ier = gotoZone();
	RETURN_IF_ERR;

	int usize;
	ier = cg_nuser_data(&usize);
	RETURN_IF_ERR;
	for (int U = 1; U <= usize; ++U) {
		char name[NAME_MAXLENGTH];
		cg_user_data_read(U, name);
		if (GCNODE == name) {return 0;}
	}
	return cg_user_data_write(GCNODE.c_str());
}

cgsize_t CgnsFile::Impl::gridNodeValueCount()
{
	if (m_baseCellDim == 1) {
		return m_zoneSize[0];
	} else if (m_baseCellDim == 2) {
		return m_zoneSize[0] * m_zoneSize[1];
	} else if (m_baseCellDim == 3) {
		return m_zoneSize[0] * m_zoneSize[1] * m_zoneSize[2];
	}
	return 1;
}

cgsize_t CgnsFile::Impl::gridCellValueCount()
{
	if (m_baseCellDim == 1) {
		return m_zoneSize[1];
	} else if (m_baseCellDim == 2) {
		return m_zoneSize[2] * m_zoneSize[3];
	} else if (m_baseCellDim == 3) {
		return m_zoneSize[3] * m_zoneSize[4] * m_zoneSize[5];
	}
	return 1;
}

int CgnsFile::Impl::gotoZoneIter()
{
	return cg_goto(m_fileId, m_baseId, "Zone_t", m_zoneId, ZINAME.c_str(), 0, NULL);
}

int CgnsFile::Impl::gotoComplexGroup(const char* groupName)
{
	return cg_goto(m_fileId, m_baseId, GCCNODE.c_str(), 0, groupName, 0, NULL);
}

int CgnsFile::Impl::gotoComplex(const char* groupName, int num)
{
	return cg_goto(m_fileId, m_baseId, GCCNODE.c_str(), 0, groupName, "UserDefinedData_t", num, NULL);
}

int CgnsFile::Impl::gotoComplexChild(const char* groupName, int num, const char* name)
{
	return cg_goto(m_fileId, m_baseId, GCCNODE.c_str(), 0, groupName, 0, "UserDefinedData_t", num, name, 0, NULL);
}

int CgnsFile::Impl::gotoComplexChildCreateIfNotExist(const char* groupName, int num, const char* name)
{
	int ier = gotoComplexGroup(groupName);
	if (ier != 0) {
		// group node does not exist. create.
		ier = cg_goto(m_fileId, m_baseId, GCCNODE.c_str(), 0, NULL);
		RETURN_IF_ERR;
		ier = cg_user_data_write(groupName);
		RETURN_IF_ERR;
	}
	ier = gotoComplex(groupName, num);
	if (ier != 0) {
		// node does not exist. create.
		ier = gotoComplexGroup(groupName);
		RETURN_IF_ERR;
		char tmpname[NAME_MAXLENGTH];
		getComplexName(num, tmpname);
		ier = cg_user_data_write(tmpname);
		RETURN_IF_ERR;
	}
	ier = gotoComplexChild(groupName, num, name);
	if (ier != 0) {
		// node does not exist. create.
		ier = gotoComplex(groupName, num);
		RETURN_IF_ERR;
		ier = cg_user_data_write(name);
		RETURN_IF_ERR;
	}
	return gotoComplexChild(groupName, num, name);
}

int CgnsFile::Impl::addComplexNodeIfNotExist()
{
	int ier = gotoBase();
	RETURN_IF_ERR;

	// try to find GridComplexCondition node.
	int usize;
	ier = cg_nuser_data(&usize);
	RETURN_IF_ERR;

	for (int U = 1; U <= usize; ++U){
		char name[NAME_MAXLENGTH];
		cg_user_data_read(U, name);
		if (GCCNODE == name) {return 0;}
	}
	// not exists. try to add node.
	return cg_user_data_write(GCCNODE.c_str());
}

void CgnsFile::Impl::getBcIndex(const char* typeName, int num, int* BC)
{
	char tmpName[NAME_MAXLENGTH];
	getBcName(typeName, num, tmpName);
	for (int idx = 0; idx < m_bcNames.size(); ++idx) {
		if (m_bcNames.at(idx) == tmpName) {
			*BC = idx + 1;
			return;
		}
	}
	*BC = 0;
}

int CgnsFile::Impl::gotoBc(const char* typeName, int num)
{
	int BC;
	getBcIndex(typeName, num, &BC);
	if (BC == 0) {
		return 1;
	}
	return cg_goto(m_fileId, m_baseId, "Zone_t", m_zoneId, "ZoneBC_t", 1, "BC_t", BC, NULL);
}

int CgnsFile::Impl::gotoBcChild(const char* typeName, int num, const char* name)
{
	int BC;
	getBcIndex(typeName, num, &BC);
	if (BC == 0) {
		return 1;
	}
	return cg_goto(m_fileId, m_baseId, "Zone_t", m_zoneId, "ZoneBC_t", 1, "BC_t", BC, name, 0, NULL);
}

int CgnsFile::Impl::gotoBcChildCreateIfNotExist(const char* typeName, int num, const char* name)
{
	int ier = gotoBcChild(typeName, num, name);
	if (ier == 0) {return 0;}

	ier = gotoBc(typeName, num);
	RETURN_IF_ERR;
	ier = cg_user_data_write(name);
	RETURN_IF_ERR;
	return gotoBcChild(typeName, num, name);
}

int CgnsFile::Impl::findArray(const char* name, int* index, DataType_t* dt, int* dim, cgsize_t* dimVec)
{
	char tmpName[NAME_MAXLENGTH];
	int narrays;

	int ier = cg_narrays(&narrays);
	RETURN_IF_ERR;

	for (int i = 1; i <= narrays; ++i) {
		ier = cg_array_info(i, tmpName, dt, dim, dimVec);
		RETURN_IF_ERR;
		if (strcmp(tmpName, name) == 0) {
			*index = i;
			return 0;
		}
	}
	return -1;
}

int CgnsFile::Impl::readArray(const char* name, DataType_t dataType, cgsize_t len, void* memory)
{
	int index;
	DataType_t dt;
	int dim;
	cgsize_t dimVec[3];

	int ier = findArray(name, &index, &dt, &dim, &(dimVec[0]));
	RETURN_IF_ERR;

	// check datatype;
	if (dataType != dt) {return -2;}
	// check datalength if needed
	if (len != -1){
		if (dim != 1 || dimVec[0] != len) {return -3;}
	}
	// found. load data.
	return cg_array_read(index, memory);
}

int CgnsFile::Impl::readArrayAs(const char* name, DataType_t dataType, size_t length, void* memory)
{
	int index;
	DataType_t dt;
	int dim;
	cgsize_t dimVec[3];

	int ier = findArray(name, &index, &dt, &dim, &(dimVec[0]));
	RETURN_IF_ERR;

	// check datalength if needed
	if (length != -1){
		if (dim != 1 || dimVec[0] != length) { return -3; }
	}
	// found. load data.
	return cg_array_read_as(index, dataType, memory);
}

int CgnsFile::Impl::readStringLen(const char* name, int* length)
{
	int index;
	DataType_t datatype;
	int dim;
	int dimVec[3];

	int ier = findArray(name, &index, &datatype, &dim, &(dimVec[0]));
	RETURN_IF_ERR;

	if (datatype != Character){return -1;}
	if (dim != 1){return -2;}
	*length = dimVec[0];

	return 0;
}

int CgnsFile::Impl::readString(const char* name, size_t bufferLen, char* buffer)
{
	int index;
	DataType_t datatype;
	int dim;
	cgsize_t dimVec[3];

	int ier = findArray(name, &index, &datatype, &dim, &(dimVec[0]));

	// check datatype
	if (datatype != Character){return -1;}
	if (bufferLen != -1){
		if (dim != 1 || (size_t)(dimVec[0]) >= bufferLen){ return -2; }
	}

	// found. load data.
	ier = cg_array_read(index, buffer);
	RETURN_IF_ERR;
	*(buffer + dimVec[0]) = '\0';
	return 0;
}

int CgnsFile::Impl::writeArray(const char* name, DataType_t dt, size_t length, void* memory)
{
	int dim = 1;
	cgsize_t dimvec = static_cast<cgsize_t> (length);
	// If array named arrayname exists, delete it first. It succeeds only when the node exist.
	cg_delete_node(name);

	if (length == 0){
		// If the length of the array is 0, cg_array_write fails.
		return 0;
	}
	return cg_array_write(name, dt, dim, &dimvec, memory);
}

int CgnsFile::Impl::writeString(const char* name, char* value)
{
	size_t length = strlen(value);
	return writeArray(name, Character, length, value);
}

void CgnsFile::Impl::getComplexName(int num, char* name)
{
	sprintf(name, "Item%d", num);
}

void CgnsFile::Impl::getDimensionArrayName(const char* dimName, char* name)
{
	sprintf(name, "%s_%s", "Dimension", dimName);
}

void CgnsFile::Impl::getFunctionalDataName(int num, char* name)
{
	if (num == 1){
		strcpy(name, "Value");
	} else {
		sprintf(name, "Value%d", num - 1);
	}
}

void CgnsFile::Impl::getBcName(const char* typeName, int num, char* name)
{
	sprintf(name, "%s_%d", typeName, num);
}

// public interfaces

CgnsFile::CgnsFile() :
	impl {new Impl()}
{}

CgnsFile::~CgnsFile()
{
	delete impl;
}

void CgnsFile::setFileId(int fileId)
{
	impl->m_fileId = fileId;
}

int CgnsFile::Init()
{
	cg_configure(CG_CONFIG_COMPRESS, 0);

	return impl->initBaseId(true, NULL);
}

int CgnsFile::InitRead_Base(char* bname)
{
	cg_configure(CG_CONFIG_COMPRESS, 0);

	return impl->initBaseId(false, bname);
}

int CgnsFile::InitRead()
{
	return InitRead_Base(NULL);
}

int CgnsFile::GotoBase(int* B)
{
	int ier = impl->initBaseId(false);
	RETURN_IF_ERR;

	ier = impl->gotoBase();
	RETURN_IF_ERR;

	*B = impl->m_baseId;
	return 0;
}

int CgnsFile::GotoCC()
{
	int ier = impl->initBaseId(false);
	RETURN_IF_ERR;

	ier = impl->gotoCC();
	if (ier == 0) {return 0;}

	// create node.
	ier = impl->gotoCCBase();
	RETURN_IF_ERR;
	ier = cg_user_data_write(CCNODE.c_str());
	RETURN_IF_ERR;
	return impl->gotoCC();
}

int CgnsFile::GotoRawDataTop()
{
	int ier = impl->initBaseId(false);
	RETURN_IF_ERR;

	ier = impl->gotoCCBase();
	// delte RawData node first.
	cg_delete_node(RDNODE.c_str());
	// create RawData node again.
	ier = cg_user_data_write(RDNODE.c_str());
	RETURN_IF_ERR;

	return impl->gotoGeoData();
}

int CgnsFile::Set_ZoneId(int zoneid)
{
	impl->m_zoneId = zoneid;
	return 0;
}

int CgnsFile::Complex_CC_Clear_Complex()
{
	int ier = impl->gotoBase();
	RETURN_IF_ERR;
	cg_delete_node(GCCNODE.c_str());

	return cg_user_data_write(GCCNODE.c_str());
}
