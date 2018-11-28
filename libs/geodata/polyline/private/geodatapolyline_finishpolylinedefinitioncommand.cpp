#include "geodatapolyline_finishpolylinedefinitioncommand.h"
#include "../geodatapolylineimplpolyline.h"

GeoDataPolyLine::FinishPolyLineDefiningCommand::FinishPolyLineDefiningCommand(GeoDataPolyLine* polyline) :
	QUndoCommand {GeoDataPolyLine::tr("Finish Defining PolyLine")}
{
	m_polyline = polyline;
	m_targetPolyLine = m_polyline->polyLine();
}

void GeoDataPolyLine::FinishPolyLineDefiningCommand::redo()
{
	m_polyline->setMouseEventMode(meNormal);
	m_targetPolyLine->finishDefinition();
	m_polyline->updateMouseCursor(m_polyline->graphicsView());
	m_polyline->updateActionStatus();
}

void GeoDataPolyLine::FinishPolyLineDefiningCommand::undo()
{
	m_polyline->setMouseEventMode(meDefining);
	m_polyline->updateMouseCursor(m_polyline->graphicsView());
	m_polyline->updateActionStatus();
}
