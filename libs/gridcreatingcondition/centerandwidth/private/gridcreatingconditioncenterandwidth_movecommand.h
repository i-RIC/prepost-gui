#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVECOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVECOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <QUndoCommand>
#include <QPointF>

class GridCreatingConditionCenterAndWidth::MoveCommand : public QUndoCommand
{
public:
	MoveCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionCenterAndWidth* cond);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	GridCreatingConditionCenterAndWidth* m_condition;
	QPointF m_offset;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVECOMMAND_H
