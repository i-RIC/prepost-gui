#ifndef GRIDCREATINGCONDITIONLAPLACE_UPDATEEDGELINESFOREDGESELECTIONCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_UPDATEEDGELINESFOREDGESELECTIONCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QUndoCommand>

class GridCreatingConditionLaplace::UpdateEdgeLinesForEdgeSelectionCommand : public QUndoCommand
{
public:
	UpdateEdgeLinesForEdgeSelectionCommand(QUndoCommand* comm, GridCreatingConditionLaplace::Impl* impl);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* comm) override;

private:
	QUndoCommand* m_comm;
	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_UPDATEEDGELINESFOREDGESELECTIONCOMMAND_H
