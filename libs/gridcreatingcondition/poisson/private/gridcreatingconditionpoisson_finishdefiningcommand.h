#ifndef GRIDCREATINGCONDITIONPOISSON_FINISHDEFININGCOMMAND_H
#define GRIDCREATINGCONDITIONPOISSON_FINISHDEFININGCOMMAND_H

#include "../gridcreatingconditionpoisson.h"

#include <QUndoCommand>

class GridCreatingConditionPoisson::FinishDefiningCommand : public QUndoCommand
{
public:
	FinishDefiningCommand(GridCreatingConditionPoisson* condition);

	void redo() override;
	void undo() override;

private:
	PolyLineController* m_targetLine;
	GridCreatingConditionPoisson* m_condition;
};

#endif // GRIDCREATINGCONDITIONPOISSON_FINISHDEFININGCOMMAND_H
