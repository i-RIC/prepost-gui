#include "gridcreatingconditioncenterandwidth_finishdefiningcommand.h"

#include <guicore/pre/base/preprocessorwindowinterface.h>

GridCreatingConditionCenterAndWidth::FinishDefiningCommand::FinishDefiningCommand(GridCreatingConditionCenterAndWidth* condition) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Finish Defining Polyline")},
	m_condition {condition}
{}

void GridCreatingConditionCenterAndWidth::FinishDefiningCommand::redo()
{
	m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meNormal;
	m_condition->updateMouseCursor(m_condition->graphicsView());
	m_condition->updateActionStatus();
	m_condition->create(m_condition->preProcessorWindow());
}

void GridCreatingConditionCenterAndWidth::FinishDefiningCommand::undo()
{
	m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meDefining;
	m_condition->updateMouseCursor(m_condition->graphicsView());
	m_condition->updateActionStatus();
}
