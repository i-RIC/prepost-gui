#include "pointcontroller.h"
#include "pointmovecommand.h"

#include <misc/qundocommandhelper.h>

PointMoveCommand::PointMoveCommand(bool keyDown, const QPointF& p, PointController* point) :
	PointMoveCommand {tr("Move Point"), keyDown, p, point}
{}

PointMoveCommand::PointMoveCommand(const QString& title, bool keyDown, const QPointF& p, PointController* point) :
	QUndoCommand {title},
	m_keyDown {keyDown},
	m_newPoint {p},
	m_oldPoint {point->point()},
	m_point {point}
{}

void PointMoveCommand::redo()
{
	m_point->setPoint(m_newPoint);
}

void PointMoveCommand::undo()
{
	m_point->setPoint(m_oldPoint);
}

int PointMoveCommand::id() const
{
	return iRIC::generateCommandId("PointMove");
}

bool PointMoveCommand::mergeWith(const QUndoCommand *other)
{
	const auto comm = dynamic_cast<const PointMoveCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_point != comm->m_point) {return false;}

	m_newPoint = comm->m_newPoint;
	return true;
}
