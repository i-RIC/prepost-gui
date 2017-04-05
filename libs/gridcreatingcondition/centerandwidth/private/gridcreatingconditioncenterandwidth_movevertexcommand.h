#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVEVERTEXCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <QUndoCommand>
#include <QPointF>

class GridCreatingConditionCenterAndWidth::MoveVertexCommand : public QUndoCommand
{
public:
	MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QPointF m_offset;
	GridCreatingConditionCenterAndWidth* m_condition;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVEVERTEXCOMMAND_H
