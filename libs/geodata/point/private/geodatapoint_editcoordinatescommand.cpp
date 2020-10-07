#include "geodatapoint_editcoordinatescommand.h"

GeoDataPoint::EditCoordinatesCommand::EditCoordinatesCommand(bool apply, const QPointF& p, GeoDataPoint* point) :
	QUndoCommand {GeoDataPoint::tr("Edit Point Coordinates")},
	m_apply {apply},
	m_newPoint {p},
	m_point {point}
{
	m_oldPoint = point->point();
}

void GeoDataPoint::EditCoordinatesCommand::redo()
{
	applyCoords(m_newPoint);

	m_point->renderGraphicsView();
}

void GeoDataPoint::EditCoordinatesCommand::undo()
{
	applyCoords(m_oldPoint);
	if (m_apply) {return;}

	m_point->renderGraphicsView();
}

void GeoDataPoint::EditCoordinatesCommand::applyCoords(const QPointF& point)
{
	m_point->setPoint(point);
}
