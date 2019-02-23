#ifndef GRIDCREATINGCONDITIONLAPLACE_CENTERLINEUPDATELABELSANDSPLINECOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_CENTERLINEUPDATELABELSANDSPLINECOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QUndoCommand>

#include <memory>

class GridCreatingConditionLaplace::CenterLineUpdateLabelsAndSplineCommand : public QUndoCommand
{
public:
	CenterLineUpdateLabelsAndSplineCommand(QUndoCommand* comm, GridCreatingConditionLaplace::Impl* impl);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_CENTERLINEUPDATELABELSANDSPLINECOMMAND_H
