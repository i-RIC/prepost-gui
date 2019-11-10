#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_DEFINENEWPOINTCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_DEFINENEWPOINTCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <guibase/polyline/polylinepushvertexcommand.h>

class GridCreatingConditionCenterAndWidth::PushVertexCommand : public PolyLinePushVertexCommand
{
public:
	PushVertexCommand(bool keyDown, const QPoint& point, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

private:
	GridCreatingConditionCenterAndWidth* m_condition;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_DEFINENEWPOINTCOMMAND_H
