#include "geodatapolyline_editcoordinatescommand.h"
#include "../geodatapolylineimplpolyline.h"

#include <guibase/vtktool/vtkpointsutil.h>

#include <vtkPolyLine.h>

GeoDataPolyLine::EditCoordinatesCommand::EditCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GeoDataPolyLine* pol) :
	QUndoCommand {GeoDataPolyLine::tr("Edit PolyLine Coordinates")},
	m_apply {apply},
	m_newCoords(coords),
	m_selectedPolyLine {pol->polyLine()},
	m_polyline {pol}
{
	m_oldCoords = m_selectedPolyLine->polyLine();
}

void GeoDataPolyLine::EditCoordinatesCommand::redo()
{
	applyCoords(m_newCoords);

	m_polyline->renderGraphicsView();
}

void GeoDataPolyLine::EditCoordinatesCommand::undo()
{
	applyCoords(m_oldCoords);
	if (m_apply) {return;}

	m_polyline->renderGraphicsView();
}

void GeoDataPolyLine::EditCoordinatesCommand::applyCoords(const std::vector<QPointF>& coords)
{
	m_selectedPolyLine->setPolyLine(coords);
}
