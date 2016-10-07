#include "iriclib_cgnsfile.h"
#include "private/iriclib_cgnsfile_impl.h"

#include <string>

#define RETURN_IF_ERR if (ier != 0) {return ier;}

using namespace iRICLib;

namespace {

static const int NAME_MAXLENGTH = 200;
static const int ZONESIZE_LENGTH = 9;
static const int FILEID_MAX = 100;
static const int ARRAYINCREMENTSTEP = 3;

static const std::string IRICBASE = "iRIC";
static const std::string IRICZONE = "iRICZone";
static const std::string iRICBASE1D = "iRIC1D";
static const std::string iRICBASE2D = "iRIC2D";
static const std::string iRICBASE3D = "iRIC3D";
static const std::string BINAME = "BaseIterativeData";
static const std::string ZINAME = "ZoneIterativeData";

static const std::string CCNODE = "CalculationConditions";
static const std::string RDNODE = "GeographicData";
static const std::string GCNODE = "GridConditions";
static const std::string GCCNODE = "GridComplexConditions";
static const std::string ECNODE = "ErrorCode";

} // namespace

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

int CgnsFile::Impl::gotoCCBaseIter()
{
	return cg_goto(m_fileId, m_ccBaseId, "BaseIterativeData_t", 1, NULL);
}

int CgnsFile::Impl::gotoZone()
{
	return cg_goto(m_fileId, m_baseId, "Zone_t", m_zoneId, NULL);
}

int CgnsFile::Impl::gotoZoneIter()
{
	return cg_goto(m_fileId, m_baseId, "Zone_t", m_zoneId, ZINAME.c_str(), 0, NULL);
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

}

int CgnsFile::GotoRawDataTop()
{

}

int CgnsFile::Set_ZoneId(int zoneid)
{

}
