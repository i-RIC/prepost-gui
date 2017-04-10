#include "gridcreatingconditionpoisson_finishdefiningcommand.h"
#include "gridcreatingconditionpoisson_impl.h"

GridCreatingConditionPoisson::FinishDefiningCommand::FinishDefiningCommand(GridCreatingConditionPoisson* condition) :
	QUndoCommand {GridCreatingConditionPoisson::tr("Finish Defining Polyline")},
	m_targetLine {condition->impl->m_activeLine},
	m_condition {condition}
{}

void GridCreatingConditionPoisson::FinishDefiningCommand::redo()
{
	m_condition->impl->m_activeLine = m_targetLine;
	m_condition->impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
	m_condition->impl->updateMouseCursor(m_condition->graphicsView());
}

void GridCreatingConditionPoisson::FinishDefiningCommand::undo()
{
	m_condition->impl->m_activeLine = m_targetLine;
	m_condition->impl->m_mouseEventMode = Impl::MouseEventMode::Defining;
	m_condition->impl->updateMouseCursor(m_condition->graphicsView());
}
