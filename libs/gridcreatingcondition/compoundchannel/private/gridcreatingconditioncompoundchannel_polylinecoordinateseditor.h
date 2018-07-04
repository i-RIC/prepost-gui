#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_POLYLINECOORDINATESEDITOR_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_POLYLINECOORDINATESEDITOR_H

#include "../gridcreatingconditioncompoundchannel.h"

class GridCreatingConditionCompoundChannel::PolylineCoordinatesEditor
{
public:
	static void edit(GridCreatingConditionCompoundChannel* condition);

private:
	static void acceptCallback(const std::vector<QPointF>& coords);
	static void rejectCallback();
	static void applyCallback(const std::vector<QPointF>& coords);
	static std::vector<QPointF> applyOffset(const std::vector<QPointF>& coords);
	static std::vector<QPointF> unapplyOffset(const std::vector<QPointF>& coords);

	static GridCreatingConditionCompoundChannel* targetCondition;
	static bool applied;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_POLYLINECOORDINATESEDITOR_H
