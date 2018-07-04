#ifndef GRIDCREATINGCONDITIONTRIANGLE_POLYLINECOORDINATESEDITOR_H
#define GRIDCREATINGCONDITIONTRIANGLE_POLYLINECOORDINATESEDITOR_H

#include "../gridcreatingconditiontriangle.h"

class GridCreatingConditionTriangle::PolylineCoordinatesEditor
{
public:
	static void edit(GridCreatingConditionTriangle* condition);

private:
	static void acceptCallback(const std::vector<QPointF>& coords);
	static void rejectCallback();
	static void applyCallback(const std::vector<QPointF>& coords);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords);

	static GridCreatingConditionTriangle* targetCondition;
	static bool applied;
};

#endif // GRIDCREATINGCONDITIONTRIANGLE_POLYLINECOORDINATESEDITOR_H
