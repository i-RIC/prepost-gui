#ifndef GRIDCREATINGCONDITIONPOISSON_ADDVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONPOISSON_ADDVERTEXCOMMAND_H

#include "../gridcreatingconditionpoisson.h"

#include <QUndoCommand>
#include <QPointF>

class PolyLineController;

class GridCreatingConditionPoisson::AddVertexCommand : public QUndoCommand
{
public:
	AddVertexCommand(bool keyDown, int edgeId, QPoint point, GridCreatingConditionPoisson* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	int m_vertexId;
	QPointF m_vertexPosition;
	PolyLineController* m_polyLine;
};

#endif // GRIDCREATINGCONDITIONPOISSON_ADDVERTEXCOMMAND_H
