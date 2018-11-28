#include "geodatapolygon_editcoordinatescommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guibase/vtktool/vtkpointsutil.h>

#include <vtkPolygon.h>

GeoDataPolygon::EditCoordinatesCommand::EditCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GeoDataPolygon* pol) :
	QUndoCommand {GeoDataPolygon::tr("Edit Polygon Coordinates")},
	m_apply {apply},
	m_newCoords(coords),
	m_selectedPolygon {pol->selectedPolygon()},
	m_polygon {pol}
{
	m_oldCoords = vtkPointsUtil::getCoordinates(m_selectedPolygon->getVtkPolygon());
}

void GeoDataPolygon::EditCoordinatesCommand::redo()
{
	applyCoords(m_newCoords);

	m_polygon->renderGraphicsView();
}

void GeoDataPolygon::EditCoordinatesCommand::undo()
{
	applyCoords(m_oldCoords);
	if (m_apply) {return;}

	m_polygon->renderGraphicsView();
}

void GeoDataPolygon::EditCoordinatesCommand::applyCoords(const std::vector<QPointF>& coords)
{
	vtkPointsUtil::setCoordinates(m_selectedPolygon->getVtkPolygon(), coords);
	m_selectedPolygon->applyVtkPolygonShape();
}
