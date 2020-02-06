#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_ADDVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_ADDVERTEXCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <guibase/polyline/polylineaddvertexcommand.h>

class GridCreatingConditionCenterAndWidth::AddVertexCommand : public PolyLineAddVertexCommand
{
public:
	AddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	GridCreatingConditionCenterAndWidth* m_condition;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_ADDVERTEXCOMMAND_H
