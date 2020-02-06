#include "gridcreatingconditioncenterandwidth_editcoordinatescommand.h"

GridCreatingConditionCenterAndWidth::EditCoordinatesCommand::EditCoordinatesCommand(const std::vector<QPointF>& coords, GridCreatingConditionCenterAndWidth* cond) :
	PolyLineEditCoordinatesCommand {tr("Edit Center Line Coordinates"), coords, &(cond->m_polyLineController)}
{}
