#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_EDITCOORDINATESCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_EDITCOORDINATESCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <guibase/polyline/polylineeditcoordinatescommand.h>

class GridCreatingConditionCenterAndWidth::EditCoordinatesCommand : public PolyLineEditCoordinatesCommand
{
public:
	EditCoordinatesCommand(const std::vector<QPointF>& coords, GridCreatingConditionCenterAndWidth* cond);
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_EDITCOORDINATESCOMMAND_H
