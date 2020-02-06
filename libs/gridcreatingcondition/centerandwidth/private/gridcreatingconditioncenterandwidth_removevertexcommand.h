#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_REMOVEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_REMOVEVERTEXCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <guibase/polyline/polylineremovevertexcommand.h>

class GridCreatingConditionCenterAndWidth::RemoveVertexCommand : public PolyLineRemoveVertexCommand
{
public:
	RemoveVertexCommand(vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

private:
	GridCreatingConditionCenterAndWidth* m_condition;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_REMOVEVERTEXCOMMAND_H
