#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVECOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVECOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <QUndoCommand>
#include <QVector2D>

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
	QVector2D m_offset;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_MOVECOMMAND_H
