#ifndef GRIDCREATINGCONDITIONTRIANGLEGRIDREGIONPOLYGON_H
#define GRIDCREATINGCONDITIONTRIANGLEGRIDREGIONPOLYGON_H

#include "gridcreatingconditiontriangleabstractpolygon.h"

class GridCreatingConditionTriangleGridRegionPolygon : public GridCreatingConditionTriangleAbstractPolygon
{
	Q_OBJECT
public:
	GridCreatingConditionTriangleGridRegionPolygon(GridCreatingConditionTriangle* parent);
};

#endif // GRIDCREATINGCONDITIONTRIANGLEGRIDREGIONPOLYGON_H
