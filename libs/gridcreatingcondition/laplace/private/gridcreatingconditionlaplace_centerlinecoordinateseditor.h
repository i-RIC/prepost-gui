#ifndef GRIDCREATINGCONDITIONLAPLACE_CENTERLINECOORDINATESEDITOR_H
#define GRIDCREATINGCONDITIONLAPLACE_CENTERLINECOORDINATESEDITOR_H

#include "../gridcreatingconditionlaplace.h"

class PolyLineController;

class GridCreatingConditionLaplace::CenterLineCoordinatesEditor
{
public:
	static void edit(PolyLineController* line, GridCreatingConditionLaplace* condition);

private:
	static void acceptCallback(const std::vector<QPointF>& coords);
	static void rejectCallback();
	static void applyCallback(const std::vector<QPointF>& coords);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords);

	static PolyLineController* targetLine;
	static GridCreatingConditionLaplace* targetCondition;
	static bool applied;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_CENTERLINECOORDINATESEDITOR_H
