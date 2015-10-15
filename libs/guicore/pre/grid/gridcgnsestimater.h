#ifndef GRIDCGNSESTIMATER_H
#define GRIDCGNSESTIMATER_H

#include "../../guicore_global.h"

class Grid;
class ProjectDataItem;

/// Read cgns file and estimate the grid type.
class GUICOREDLL_EXPORT GridCgnsEstimater
{
public:
	static Grid* buildGrid(int fn, int baseId, int zoneId, ProjectDataItem* parent);

private:
	GridCgnsEstimater();
};

#endif // GRIDCGNSESTIMATER_H
