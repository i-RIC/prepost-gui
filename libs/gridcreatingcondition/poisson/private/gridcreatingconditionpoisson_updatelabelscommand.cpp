#include "gridcreatingconditionpoisson_impl.h"
#include "gridcreatingconditionpoisson_updatelabelscommand.h"

GridCreatingConditionPoisson::UpdateLabelsCommand::UpdateLabelsCommand(QUndoCommand* com, GridCreatingConditionPoisson* condition) :
	m_command {com},
	m_condition {condition}
{}

void GridCreatingConditionPoisson::UpdateLabelsCommand::redo()
{
	m_command->redo();
	m_condition->impl->updateLabels();
}

void GridCreatingConditionPoisson::UpdateLabelsCommand::undo()
{
	m_command->undo();
	m_condition->impl->updateLabels();
}

int GridCreatingConditionPoisson::UpdateLabelsCommand::id() const
{
	return m_command->id();
}

bool GridCreatingConditionPoisson::UpdateLabelsCommand::mergeWith(const QUndoCommand *other)
{
	auto c = dynamic_cast<const UpdateLabelsCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
