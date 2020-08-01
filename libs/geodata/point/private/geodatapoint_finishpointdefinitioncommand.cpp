#include "geodatapoint_finishpointdefinitioncommand.h"

GeoDataPoint::FinishPointDefiningCommand::FinishPointDefiningCommand(GeoDataPoint* point) :
	QUndoCommand {GeoDataPoint::tr("Finish Defining Point")}
{
	m_point = point;
}

void GeoDataPoint::FinishPointDefiningCommand::redo()
{
	m_point->setMouseEventMode(meNormal);
	// m_point->finishDefinition();
	m_point->updateMouseCursor(m_point->graphicsView());
}

void GeoDataPoint::FinishPointDefiningCommand::undo()
{
	m_point->setMouseEventMode(meDefining);
	m_point->updateMouseCursor(m_point->graphicsView());
}
