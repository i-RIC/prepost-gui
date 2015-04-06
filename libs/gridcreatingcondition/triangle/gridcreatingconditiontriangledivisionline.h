#ifndef GRIDCREATINGCONDITIONTRIANGLEDIVISIONLINE_H
#define GRIDCREATINGCONDITIONTRIANGLEDIVISIONLINE_H

#include "gridcreatingconditiontriangleabstractline.h"

class GridCreatingConditionTriangleDivisionLine : public GridCreatingConditionTriangleAbstractLine
{
	Q_OBJECT
public:
	GridCreatingConditionTriangleDivisionLine(GridCreatingConditionTriangle* parent);
};

#endif // GRIDCREATINGCONDITIONTRIANGLEDIVISIONLINE_H
