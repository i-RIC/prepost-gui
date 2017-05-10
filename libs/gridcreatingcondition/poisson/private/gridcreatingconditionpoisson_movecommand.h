#ifndef GRIDCREATINGCONDITIONPOISSON_MOVECOMMAND_H
#define GRIDCREATINGCONDITIONPOISSON_MOVECOMMAND_H

#include "../gridcreatingconditionpoisson.h"

#include <QUndoCommand>
#include <QPointF>

class GridCreatingConditionPoisson::MoveCommand : public QUndoCommand
{
public:
	MoveCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionPoisson* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	PolyLineController* m_polyLine;
	QPointF m_offset;
};

#endif // GRIDCREATINGCONDITIONPOISSON_MOVECOMMAND_H
