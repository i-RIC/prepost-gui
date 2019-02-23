#include "gridcreatingconditionlaplace_ctrlpointmovecommand.h"
#include "gridcreatingconditionlaplace_impl.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionLaplace::CtrlPointMoveCommand::CtrlPointMoveCommand(bool keyDown, const QPoint& from, const QPoint& to, int pointId, GridCreatingConditionLaplace* cond) :
	QUndoCommand(GridCreatingConditionLaplace::tr("Move Point")),
	m_keyDown {keyDown},
	m_pointId {pointId},
	m_condition {cond}
{
	m_oldPosition = m_condition->impl->m_ctrlPoints[m_pointId];

	double x = from.x();
	double y = from.y();
	cond->graphicsView()->viewportToWorld(x, y);
	QPointF fromVec(x, y);
	x = to.x();
	y = to.y();
	cond->graphicsView()->viewportToWorld(x, y);
	QPointF toVec(x, y);

	m_newPosition = m_oldPosition + toVec - fromVec;
}

void GridCreatingConditionLaplace::CtrlPointMoveCommand::redo()
{
	m_condition->impl->m_ctrlPoints[m_pointId] = m_newPosition;
}

void GridCreatingConditionLaplace::CtrlPointMoveCommand::undo()
{
	m_condition->impl->m_ctrlPoints[m_pointId] = m_oldPosition;
}

int GridCreatingConditionLaplace::CtrlPointMoveCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionLaplaceCtrlPointMove");
}

bool GridCreatingConditionLaplace::CtrlPointMoveCommand::mergeWith(const QUndoCommand* other)
{
	const auto* comm = dynamic_cast<const CtrlPointMoveCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_pointId != m_pointId) {return false;}

	m_newPosition = comm->m_newPosition;
	return true;
}
