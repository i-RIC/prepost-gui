#include "gridcreatingconditioncenterandwidth_editcoordinatescommand.h"
#include "gridcreatingconditioncenterandwidth_impl.h"

GridCreatingConditionCenterAndWidth::EditCoordinatesCommand::EditCoordinatesCommand(const std::vector<QPointF>& coords, GridCreatingConditionCenterAndWidth* cond) :
	PolyLineEditCoordinatesCommand {tr("Edit Center Line Coordinates"), coords, &(cond->impl->m_polyLineController)}
{}
