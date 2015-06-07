#ifndef GRIDCREATINGCONDITIONTRIANGLEHOLEPOLYGON_H
#define GRIDCREATINGCONDITIONTRIANGLEHOLEPOLYGON_H

#include "gridcreatingconditiontriangleabstractpolygon.h"

class GridCreatingConditionTriangleHolePolygon : public GridCreatingConditionTriangleAbstractPolygon
{
	Q_OBJECT

public:
	GridCreatingConditionTriangleHolePolygon(GridCreatingConditionTriangle* parent);
};

#endif // GRIDCREATINGCONDITIONTRIANGLEHOLEPOLYGON_H
