#include "gridcreatingconditionlaplace_centerlinefinishdefiningcommand.h"
#include "gridcreatingconditionlaplace_impl.h"

GridCreatingConditionLaplace::CenterLineFinishDefiningCommand::CenterLineFinishDefiningCommand(GridCreatingConditionLaplace* cond) :
	QUndoCommand(GridCreatingConditionLaplace::tr("Finish Defining Center Line")),
	m_condition {cond}
{}

void GridCreatingConditionLaplace::CenterLineFinishDefiningCommand::redo()
{
	m_condition->impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Normal;
}

void GridCreatingConditionLaplace::CenterLineFinishDefiningCommand::undo()
{
	m_condition->impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Defining;
}
