#include "pointcontroller.h"
#include "pointsetcommand.h"

#include <misc/qundocommandhelper.h>

PointSetCommand::PointSetCommand(bool keyDown, const QPointF& p, PointController* point) :
	PointSetCommand {tr("Set Point"), keyDown, p, point}
{}

PointSetCommand::PointSetCommand(const QString& title, bool keyDown, const QPointF& p, PointController* point) :
	QUndoCommand {title},
	m_keyDown {keyDown},
	m_newPoint {p},
	m_point {point}
{}

void PointSetCommand::redo()
{
	m_point->setPoint(m_newPoint);
}

void PointSetCommand::undo()
{
	m_point->clear();
}

int PointSetCommand::id() const
{
	return iRIC::generateCommandId("PointSet");
}

bool PointSetCommand::mergeWith(const QUndoCommand *other)
{
	const auto comm = dynamic_cast<const PointSetCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_point != comm->m_point) {return false;}

	m_newPoint = comm->m_newPoint;
	return true;
}
