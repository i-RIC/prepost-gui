#ifndef GRIDCGNSESTIMATER_H
#define GRIDCGNSESTIMATER_H

#include "../../guicore_global.h"

class Grid;
class ProjectDataItem;

namespace iRICLib {
	class H5CgnsZone;
} // namespace iRICLib

/// Read cgns file and estimate the grid type.
class GUICOREDLL_EXPORT GridCgnsEstimater
{
public:
	static Grid* buildGrid(const iRICLib::H5CgnsZone& zone, ProjectDataItem* parent);

private:
	GridCgnsEstimater();
};

#endif // GRIDCGNSESTIMATER_H
