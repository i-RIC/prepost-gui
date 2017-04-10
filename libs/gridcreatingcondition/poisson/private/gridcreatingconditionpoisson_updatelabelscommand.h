#ifndef GRIDCREATINGCONDITIONPOISSON_UPDATELABELSCOMMAND_H
#define GRIDCREATINGCONDITIONPOISSON_UPDATELABELSCOMMAND_H

#include "../gridcreatingconditionpoisson.h"

#include <QUndoCommand>

#include <memory>

class GridCreatingConditionPoisson::UpdateLabelsCommand : public QUndoCommand
{
public:
	UpdateLabelsCommand(QUndoCommand* com, GridCreatingConditionPoisson* condition);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GridCreatingConditionPoisson* m_condition;
};

#endif // GRIDCREATINGCONDITIONPOISSON_UPDATELABELSCOMMAND_H
