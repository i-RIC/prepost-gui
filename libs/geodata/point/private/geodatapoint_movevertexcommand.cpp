#include "geodatapoint_movevertexcommand.h"

#include <misc/qundocommandhelper.h>

GeoDataPoint::MoveVertexCommand::MoveVertexCommand(bool keyDown, const QPointF& p, GeoDataPoint* point) :
	QUndoCommand(GeoDataPoint::tr("Move vertex")),
	m_newPoint {p},
	m_oldPoint {point->point()},
	m_keyDown {keyDown},
	m_point {point}
{}

void GeoDataPoint::MoveVertexCommand::redo()
{
	apply(m_newPoint);
}

void GeoDataPoint::MoveVertexCommand::undo()
{
	apply(m_oldPoint);
}

int GeoDataPoint::MoveVertexCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPointMoveVertex");
}

bool GeoDataPoint::MoveVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const MoveVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}

	m_newPoint = comm->m_newPoint;
	return true;
}

void GeoDataPoint::MoveVertexCommand::apply(const QPointF& point)
{
	m_point->setPoint(point);
	m_point->updateScalarValues();
	m_point->updateActorSetting();
}
