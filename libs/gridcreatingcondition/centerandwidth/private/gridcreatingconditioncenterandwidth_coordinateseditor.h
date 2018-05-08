#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_COORDINATESEDITOR_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_COORDINATESEDITOR_H

#include "../gridcreatingconditioncenterandwidth.h"

class GridCreatingConditionCenterAndWidth::CoordinatesEditor
{
public:
	static void edit(GridCreatingConditionCenterAndWidth* cond);

private:
	static void acceptCallback(const std::vector<QPointF>& coords);
	static void rejectCallback();
	static void applyCallback(const std::vector<QPointF>& coords);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords);

	static GridCreatingConditionCenterAndWidth* targetCond;
	static bool applied;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_COORDINATESEDITOR_H
