#include "gridcreatingconditionlaplace_centerlineupdatelabelsandsplinecommand.h"
#include "gridcreatingconditionlaplace_impl.h"

GridCreatingConditionLaplace::CenterLineUpdateLabelsAndSplineCommand::CenterLineUpdateLabelsAndSplineCommand(QUndoCommand* comm, GridCreatingConditionLaplace::Impl* impl) :
	QUndoCommand(),
	m_command {comm},
	m_impl {impl}
{}

void GridCreatingConditionLaplace::CenterLineUpdateLabelsAndSplineCommand::redo()
{
	m_command->redo();
	m_impl->updateCenterLineLabelsAndSpline();
}

void GridCreatingConditionLaplace::CenterLineUpdateLabelsAndSplineCommand::undo()
{
	m_command->undo();
	m_impl->updateCenterLineLabelsAndSpline();
}

int GridCreatingConditionLaplace::CenterLineUpdateLabelsAndSplineCommand::id() const
{
	return m_command->id();
}

bool GridCreatingConditionLaplace::CenterLineUpdateLabelsAndSplineCommand::mergeWith(const QUndoCommand* other)
{
	auto c = dynamic_cast<const CenterLineUpdateLabelsAndSplineCommand*>(other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
