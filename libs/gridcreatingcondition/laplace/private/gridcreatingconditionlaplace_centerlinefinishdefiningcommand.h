#ifndef GRIDCREATINGCONDITIONLAPLACE_CENTERLINEFINISHDEFININGCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_CENTERLINEFINISHDEFININGCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QUndoCommand>

class GridCreatingConditionLaplace::CenterLineFinishDefiningCommand : public QUndoCommand
{
public:
	CenterLineFinishDefiningCommand(GridCreatingConditionLaplace* cond);

	void redo() override;
	void undo() override;

private:
	GridCreatingConditionLaplace* m_condition;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_CENTERLINEFINISHDEFININGCOMMAND_H
