#include "pointcontroller.h"
#include "pointclearcommand.h"

PointClearCommand::PointClearCommand(PointController* point) :
	PointClearCommand {tr("Clear Point"), point}
{}

PointClearCommand::PointClearCommand(const QString& title, PointController* point) :
	QUndoCommand {title},
	m_oldPoint {point->point()}
{}

void PointClearCommand::redo()
{
	m_point->clear();
}

void PointClearCommand::undo()
{
	m_point->setPoint(m_oldPoint);
}
