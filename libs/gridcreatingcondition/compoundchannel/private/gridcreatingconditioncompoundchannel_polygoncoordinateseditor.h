#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_POLYGONCOORDINATESEDITOR_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_POLYGONCOORDINATESEDITOR_H

#include "../gridcreatingconditioncompoundchannel.h"

class GridCreatingConditionCompoundChannel::PolygonCoordinatesEditor
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

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNEL_POLYGONCOORDINATESEDITOR_H
