#include "gridcreatingconditioncenterandwidth_finishdefiningcommand.h"
#include "gridcreatingconditioncenterandwidth_impl.h"

#include <guicore/pre/base/preprocessorwindowi.h>

GridCreatingConditionCenterAndWidth::FinishDefiningCommand::FinishDefiningCommand(GridCreatingConditionCenterAndWidth* condition) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Finish Defining Polyline")},
	m_condition {condition}
{}

void GridCreatingConditionCenterAndWidth::FinishDefiningCommand::redo()
{
	m_condition->impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
	m_condition->impl->updateMouseCursor(m_condition->graphicsView());
	m_condition->impl->updateActionStatus();
	m_condition->create(m_condition->preProcessorWindow());
}

void GridCreatingConditionCenterAndWidth::FinishDefiningCommand::undo()
{
	m_condition->impl->m_mouseEventMode = Impl::MouseEventMode::Defining;
	m_condition->impl->updateMouseCursor(m_condition->graphicsView());
	m_condition->impl->updateActionStatus();
}
