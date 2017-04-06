#ifndef GRIDCREATINGCONDITIONPOISSON_MOVEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONPOISSON_MOVEVERTEXCOMMAND_H

#include "../gridcreatingconditionpoisson.h"

#include <QUndoCommand>
#include <QPointF>

class GridCreatingConditionPoisson::MoveVertexCommand : public QUndoCommand
{
public:
	MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, int vertexId, GridCreatingConditionPoisson* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	int m_vertexId;
	QPointF m_offset;
	PolyLineController* m_polyLine;
};

#endif // GRIDCREATINGCONDITIONPOISSON_MOVEVERTEXCOMMAND_H
