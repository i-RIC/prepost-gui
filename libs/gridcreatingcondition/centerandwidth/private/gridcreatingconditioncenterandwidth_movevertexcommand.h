#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVEVERTEXCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <guibase/polyline/polylinemovevertexcommand.h>

class GridCreatingConditionCenterAndWidth::MoveVertexCommand : public PolyLineMoveVertexCommand
{
public:
	MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	GridCreatingConditionCenterAndWidth* m_condition;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVEVERTEXCOMMAND_H
