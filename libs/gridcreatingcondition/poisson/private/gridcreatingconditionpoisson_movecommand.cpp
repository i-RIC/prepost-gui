#include "gridcreatingconditionpoisson_impl.h"
#include "gridcreatingconditionpoisson_movecommand.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionPoisson::MoveCommand::MoveCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionPoisson* cond) :
	QUndoCommand {GridCreatingConditionPoisson::tr("Move Line")},
	m_keyDown {keyDown},
	m_polyLine {cond->impl->m_activeLine}
{
	double dx = from.x();
	double dy = from.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QPointF fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QPointF toVec(dx, dy);
	m_offset = toVec - fromVec;
}

void GridCreatingConditionPoisson::MoveCommand::redo()
{
	auto line = m_polyLine->polyLine();
	for (QPointF& p : line) {
		p += m_offset;
	}
	m_polyLine->setPolyLine(line);
}

void GridCreatingConditionPoisson::MoveCommand::undo()
{
	auto line = m_polyLine->polyLine();
	for (QPointF& p : line) {
		p -= m_offset;
	}
	m_polyLine->setPolyLine(line);
}

int GridCreatingConditionPoisson::MoveCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionPoissonMove");
}

bool GridCreatingConditionPoisson::MoveCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const MoveCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polyLine != m_polyLine) {return false;}

	m_offset += comm->m_offset;
	return true;
}
