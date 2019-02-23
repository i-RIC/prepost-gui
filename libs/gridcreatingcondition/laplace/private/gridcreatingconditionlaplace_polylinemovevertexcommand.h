#ifndef GRIDCREATINGCONDITIONLAPLACE_POLYLINEMOVEVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_POLYLINEMOVEVERTEXCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QPointF>
#include <QUndoCommand>

class PolyLineController;

class GridCreatingConditionLaplace::PolyLineMoveVertexCommand : public QUndoCommand
{
public:
	PolyLineMoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, PolyLineController* line, int vertexId, GridCreatingConditionLaplace* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	int m_vertexId;
	QPointF m_newPosition;
	QPointF m_oldPosition;
	PolyLineController* m_polyline;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_POLYLINEMOVEVERTEXCOMMAND_H
