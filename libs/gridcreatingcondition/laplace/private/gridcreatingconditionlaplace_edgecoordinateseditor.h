#ifndef GRIDCREATINGCONDITIONLAPLACE_EDGECOORDINATESEDITOR_H
#define GRIDCREATINGCONDITIONLAPLACE_EDGECOORDINATESEDITOR_H

#include "../gridcreatingconditionlaplace.h"

class PolyLineController;

class GridCreatingConditionLaplace::EdgeCoordinatesEditor
{
public:
	static void edit(bool streamWise, int i, int j, GridCreatingConditionLaplace* condition);

private:
	static void acceptCallback(const std::vector<QPointF>& coords);
	static void rejectCallback();
	static void applyCallback(const std::vector<QPointF>& coords);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords);

	static bool streamWise;
	static int indexI;
	static int indexJ;
	static GridCreatingConditionLaplace* targetCondition;
	static bool applied;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_EDGECOORDINATESEDITOR_H
