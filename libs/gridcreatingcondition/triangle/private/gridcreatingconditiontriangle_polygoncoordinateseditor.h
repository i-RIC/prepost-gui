#ifndef GRIDCREATINGCONDITIONTRIANGLE_POLYGONCOORDINATESEDITOR_H
#define GRIDCREATINGCONDITIONTRIANGLE_POLYGONCOORDINATESEDITOR_H

#include "../gridcreatingconditiontriangle.h"

class GridCreatingConditionTriangle::PolygonCoordinatesEditor
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

#endif // GRIDCREATINGCONDITIONTRIANGLE_POLYGONCOORDINATESEDITOR_H
