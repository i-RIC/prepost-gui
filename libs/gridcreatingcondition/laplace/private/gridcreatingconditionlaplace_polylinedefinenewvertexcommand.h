#ifndef GRIDCREATINGCONDITIONLAPLACE_POLYLINEDEFINENEWVERTEXCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_POLYLINEDEFINENEWVERTEXCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QPointF>
#include <QUndoCommand>

class PolyLineController;

class GridCreatingConditionLaplace::PolyLineDefineNewVertexCommand : public QUndoCommand
{
public:
	PolyLineDefineNewVertexCommand(bool keyDown, const QPoint& point, PolyLineController* line, GridCreatingConditionLaplace* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	QPointF m_point;
	PolyLineController* m_polyLine;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_POLYLINEDEFINENEWVERTEXCOMMAND_H
