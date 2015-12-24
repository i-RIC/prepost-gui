#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_ADDVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_ADDVERTEXCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <QUndoCommand>
#include <QVector2D>

class GridCreatingConditionCenterAndWidth::AddVertexCommand : public QUndoCommand
{
public:
	AddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GridCreatingConditionCenterAndWidth* m_condition;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_ADDVERTEXCOMMAND_H
