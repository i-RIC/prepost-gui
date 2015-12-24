#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_REMOVEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_REMOVEVERTEXCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <QUndoCommand>
#include <QVector2D>

class GridCreatingConditionCenterAndWidth::RemoveVertexCommand : public QUndoCommand
{
public:
	RemoveVertexCommand(vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

private:
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionCenterAndWidth* m_condition;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_REMOVEVERTEXCOMMAND_H
