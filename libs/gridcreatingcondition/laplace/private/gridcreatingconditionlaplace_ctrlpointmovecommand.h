#ifndef GRIDCREATINGCONDITIONLAPLACE_CTRLPOINTMOVECOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_CTRLPOINTMOVECOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QUndoCommand>
#include <QPointF>

class GridCreatingConditionLaplace::CtrlPointMoveCommand : public QUndoCommand
{
public:
	CtrlPointMoveCommand(bool keyDown, const QPoint& from, const QPoint& to, int pointId, GridCreatingConditionLaplace* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	int m_pointId;
	QPointF m_newPosition;
	QPointF m_oldPosition;
	GridCreatingConditionLaplace* m_condition;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_CTRLPOINTMOVECOMMAND_H
