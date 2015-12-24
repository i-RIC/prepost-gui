#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTH_FINISHDEFININGCOMMAND_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTH_FINISHDEFININGCOMMAND_H

#include "../gridcreatingconditioncenterandwidth.h"

#include <QUndoCommand>

class GridCreatingConditionCenterAndWidth::FinishDefiningCommand : public QUndoCommand
{
public:
	FinishDefiningCommand(GridCreatingConditionCenterAndWidth* condition);

	void redo() override;
	void undo() override;

private:
	GridCreatingConditionCenterAndWidth* m_condition;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTH_FINISHDEFININGCOMMAND_H
