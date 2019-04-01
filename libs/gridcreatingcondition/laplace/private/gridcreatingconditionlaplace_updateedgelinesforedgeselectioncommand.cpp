#include "gridcreatingconditionlaplace_impl.h"
#include "gridcreatingconditionlaplace_updateedgelinesforedgeselectioncommand.h"

GridCreatingConditionLaplace::UpdateEdgeLinesForEdgeSelectionCommand::UpdateEdgeLinesForEdgeSelectionCommand(QUndoCommand* comm, GridCreatingConditionLaplace::Impl* impl) :
	QUndoCommand(),
	m_comm {comm},
	m_impl {impl}
{}

void GridCreatingConditionLaplace::UpdateEdgeLinesForEdgeSelectionCommand::redo()
{
	m_comm->redo();
	m_impl->updateEdgeLinesForSelection();
}

void GridCreatingConditionLaplace::UpdateEdgeLinesForEdgeSelectionCommand::undo()
{
	m_comm->undo();
	m_impl->updateEdgeLinesForSelection();
}

int GridCreatingConditionLaplace::UpdateEdgeLinesForEdgeSelectionCommand::id() const
{
	return m_comm->id();
}

bool GridCreatingConditionLaplace::UpdateEdgeLinesForEdgeSelectionCommand::mergeWith(const QUndoCommand* comm)
{
	auto comm2 = dynamic_cast<const UpdateEdgeLinesForEdgeSelectionCommand*> (comm);
	if (comm2 == nullptr) {return false;}

	return m_comm->mergeWith(comm2->m_comm);
}
