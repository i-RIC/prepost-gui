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
	int clearBaseIter();
	int clearZoneIter();
	int clearSolution();
	int clearParticleSolution();
	int clearResultGrids();

	int loadResultData();
	int loadBcNames();

	int gotoBase();
	int gotoBaseIter();

	int gotoCCBase();
	int gotoCCBaseIter();

	int gotoZone();
	int gotoZoneIter();

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
