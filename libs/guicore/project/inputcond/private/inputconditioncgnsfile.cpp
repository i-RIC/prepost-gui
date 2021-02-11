#include "inputconditioncgnsfile.h"

#include <guicore/project/projectcgnsfile.h>
#include <misc/stringtool.h>
#include <misc/versionnumber.h>

#include <cgnslib.h>
#include <iriclib.h>

#include <string>

namespace {

static const int NAME_MAXLENGTH = 200;
static const std::string iRICBASE = "iRIC";

bool loadGridData(int fid, int bid, int zid, GridLocation_t location, QStringList* realResults, QStringList* integerResults)
{
	int ier;
	int nsols;
	ier = cg_nsols(fid, bid, zid, &nsols);
	if (ier != 0) {return false;}

	for (int S = 1; S <= nsols; ++S) {
		char name[NAME_MAXLENGTH];
		GridLocation_t l;
		cg_sol_info(fid, bid, zid, S, name, &l);
		if (l != location) {continue;}

		ier = cg_goto(fid, bid, "Zone_t", zid, "FlowSolution_t", S, NULL);
		if (ier != 0) {return false;}

		int narrays;
		ier = cg_narrays(&narrays);
		if (ier != 0) {return false;}

		for (int A = 1; A <= narrays; ++A) {
			DataType_t type;
			int dim;
			cgsize_t dimVec[3];
			ier = cg_array_info(A, name, &type, &dim, &(dimVec[0]));
			if (ier != 0) {return false;}

			if (type == Integer) {
				integerResults->push_back(name);
			} else if (type == RealDouble) {
				realResults->push_back(name);
			}
		}
		return true;
	}
	return true;
}

} // namespace

InputConditionCgnsFile::InputConditionCgnsFile() :
	m_fileName {},
	m_isEffective {false},
	m_baseIterativeIntegerResults {},
	m_baseIterativeRealResults {},
	m_gridNodeIntegerResults {},
	m_gridNodeRealResults {},
	m_gridCellIntegerResults {},
	m_gridCellRealResults {},
	m_gridEdgeIIntegerResults{},
	m_gridEdgeIRealResults{},
	m_gridEdgeJIntegerResults{},
	m_gridEdgeJRealResults{}
{}

void InputConditionCgnsFile::setFileName(const QString& filename)
{
	bool c = (filename != m_fileName);

	m_fileName = filename;
	if (c) {
		loadData();
		emit changed();
	}
}

bool InputConditionCgnsFile::isEffective() const
{
	return m_isEffective;
}

const InputConditionCgnsFile::SolverInformation& InputConditionCgnsFile::solverInformation() const
{
	return m_solverInformation;
}

QStringList InputConditionCgnsFile::baseIterativeIntegerResults() const
{
	return m_baseIterativeIntegerResults;
}

QStringList InputConditionCgnsFile::baseIterativeRealResults() const
{
	return m_baseIterativeRealResults;
}

QStringList InputConditionCgnsFile::gridNodeIntegerResults() const
{
	return m_gridNodeIntegerResults;
}

QStringList InputConditionCgnsFile::gridNodeRealResults() const
{
	return m_gridNodeRealResults;
}

QStringList InputConditionCgnsFile::gridCellIntegerResults() const
{
	return m_gridCellIntegerResults;
}

QStringList InputConditionCgnsFile::gridCellRealResults() const
{
	return m_gridCellRealResults;
}

QStringList InputConditionCgnsFile::gridEdgeIIntegerResults() const
{
	return m_gridEdgeIIntegerResults;
}

QStringList InputConditionCgnsFile::gridEdgeIRealResults() const
{
	return m_gridEdgeIRealResults;
}

QStringList InputConditionCgnsFile::gridEdgeJIntegerResults() const
{
	return m_gridEdgeJIntegerResults;
}

QStringList InputConditionCgnsFile::gridEdgeJRealResults() const
{
	return m_gridEdgeJRealResults;
}

bool InputConditionCgnsFile::loadData()
{
	m_isEffective = false;
	m_solverInformation.timeSteps = 0;

	m_baseIterativeIntegerResults.clear();
	m_baseIterativeRealResults.clear();
	m_gridNodeIntegerResults.clear();
	m_gridNodeRealResults.clear();
	m_gridCellIntegerResults.clear();
	m_gridCellRealResults.clear();
	m_gridEdgeIIntegerResults.clear();
	m_gridEdgeIRealResults.clear();
	m_gridEdgeJIntegerResults.clear();
	m_gridEdgeJRealResults.clear();

	std::string fname = iRIC::toStr(m_fileName);
	int ier, fid;
	ier = cg_open(fname.c_str(), CG_MODE_READ, &fid);
	if (ier != 0) {
		// opening failed. this is not a valid cgns file.
		return false;
	}

	std::string solverName;
	VersionNumber solverVersion;

	bool ok = ProjectCgnsFile::readSolverInfo(fid, &solverName, &solverVersion);
	if (ok) {
		m_solverInformation.solverName = solverName.c_str();
		m_solverInformation.solverVersion = solverVersion.toAboutString();
	}

	int bid = 0;
	int nbases;
	char name[NAME_MAXLENGTH];
	int celldim, physdim;
	ier = cg_nbases(fid, &nbases);
	if (ier != 0) {return false;}

	for (int B = 1; B <= nbases; ++B) {
		ier = cg_base_read(fid, B, name, &celldim, &physdim);
		if (ier != 0) {return false;}
		if (iRICBASE == std::string(name)) {
			bid = B;
		}
	}
	if (bid == 0) {return false;}

	// read base iterative data
	ier = cg_goto(fid, bid, "BaseIterativeData_t", 1, NULL);
	if (ier != 0) {return false;}
	int narrays;
	ier = cg_narrays(&narrays);
	if (ier != 0) {return false;}
	for (int A = 1; A <= narrays; ++A) {
		DataType_t dtype;
		int dim;
		cgsize_t dimVec;
		cg_array_info(A, name, &dtype, &dim, &dimVec);
		if (dtype == Character) {
			continue;
		} else if (dtype == RealDouble) {
			if (std::string(name) == "TimeValues") {
				m_solverInformation.timeSteps = dimVec;
				continue;
			}
			m_baseIterativeRealResults.push_back(name);
		} else if (dtype == Integer) {
			m_baseIterativeIntegerResults.push_back(name);
		}
	}

	// zone id is currently fixed to 1.
	int zid = 1;

	ZoneType_t zoneType;
	ier = cg_zone_type(fid, bid, zid, &zoneType);
	if (ier != 0) {return false;}

	cgsize_t size[9];
	ier = cg_zone_read(fid, bid, zid, name, &(size[0]));
	if (ier != 0) {return false;}

	if (zoneType == Structured) {
		m_solverInformation.gridType = SolverInformation::GridType::Structured;
		m_solverInformation.iSize = size[0];
		m_solverInformation.jSize = size[1];
	} else {
		m_solverInformation.gridType = SolverInformation::GridType::Unstructured;
		m_solverInformation.size = size[0];
	}

	ok = loadGridData(fid, bid, zid, Vertex, &m_gridNodeRealResults, &m_gridNodeIntegerResults);
	if (! ok) {return false;}
	ok = loadGridData(fid, bid, zid, CellCenter, &m_gridCellRealResults, &m_gridCellIntegerResults);
	if (! ok) {return false;}
	ok = loadGridData(fid, bid, zid, IFaceCenter, &m_gridEdgeIRealResults, &m_gridEdgeIIntegerResults);
	if (! ok) {return false;}
	ok = loadGridData(fid, bid, zid, JFaceCenter, &m_gridEdgeJRealResults, &m_gridEdgeJIntegerResults);
	if (! ok) {return false;}

	m_isEffective = true;
	cg_close(fid);
	return true;
}
