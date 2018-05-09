#include "gridcreatingconditiontriangle_editpolygoncoordinatescommand.h"
#include "../gridcreatingconditiontriangleabstractpolygon.h"

#include <guibase/vtktool/vtkpointsutil.h>

GridCreatingConditionTriangle::EditPolygonCoordinatesCommand::EditPolygonCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GridCreatingConditionTriangle* cond) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Edit Polygon Coordinates")},
	m_apply {apply},
	m_newCoords (coords),
	m_selectedPolygon {cond->m_selectedPolygon},
	m_condition {cond}
{
	m_oldCoords = vtkPointsUtil::getCoordinates(m_selectedPolygon->getVtkPolygon());
}

void GridCreatingConditionTriangle::EditPolygonCoordinatesCommand::redo()
{
	vtkPointsUtil::setCoordinates(m_selectedPolygon->getVtkPolygon(), m_newCoords);
	m_selectedPolygon->updateShapeData();

	m_condition->renderGraphicsView();
}

void GridCreatingConditionTriangle::EditPolygonCoordinatesCommand::undo()
{
	vtkPointsUtil::setCoordinates(m_selectedPolygon->getVtkPolygon(), m_oldCoords);
	m_selectedPolygon->updateShapeData();
	if (m_apply) {return;}

	m_condition->renderGraphicsView();
}
