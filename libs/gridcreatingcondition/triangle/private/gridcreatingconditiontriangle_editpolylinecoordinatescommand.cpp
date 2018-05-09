#include "gridcreatingconditiontriangle_editpolylinecoordinatescommand.h"
#include "../gridcreatingconditiontriangleabstractline.h"

#include <guibase/vtktool/vtkpointsutil.h>

GridCreatingConditionTriangle::EditPolyLineCoordinatesCommand::EditPolyLineCoordinatesCommand(bool apply, std::vector<QPointF> coords, GridCreatingConditionTriangle* cond) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Edit Break Line Coordinates")},
	m_apply {apply},
	m_newCoords (coords),
	m_selectedLine {cond->m_selectedLine},
	m_condition {cond}
{
	m_oldCoords = vtkPointsUtil::getCoordinates(m_selectedLine->getVtkLine());
}

void GridCreatingConditionTriangle::EditPolyLineCoordinatesCommand::redo()
{
	vtkPointsUtil::setCoordinates(m_selectedLine->getVtkLine(), m_newCoords);
	m_selectedLine->updateShapeData();

	m_condition->renderGraphicsView();
}

void GridCreatingConditionTriangle::EditPolyLineCoordinatesCommand::undo() {
	vtkPointsUtil::setCoordinates(m_selectedLine->getVtkLine(), m_oldCoords);
	m_selectedLine->updateShapeData();
	if (m_apply) {return;}

	m_condition->renderGraphicsView();
}
