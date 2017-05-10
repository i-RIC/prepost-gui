#ifndef GRIDCREATINGCONDITIONPOISSON_REMOVEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONPOISSON_REMOVEVERTEXCOMMAND_H

#include "../gridcreatingconditionpoisson.h"

#include <QUndoCommand>
#include <QPointF>

class GridCreatingConditionPoisson::RemoveVertexCommand : public QUndoCommand
{
public:
	RemoveVertexCommand(int vertexId, GridCreatingConditionPoisson* cond);

	void redo() override;
	void undo() override;

private:
	int m_vertexId;
	QPointF m_vertexPosition;
	PolyLineController* m_polyLine;
	GridCreatingConditionPoisson* m_condition;
};

#endif // GRIDCREATINGCONDITIONPOISSON_REMOVEVERTEXCOMMAND_H
