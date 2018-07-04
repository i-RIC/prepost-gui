#include "gridcreatingconditioncompoundchannel_editpolygoncoordinatescommand.h"
#include "../gridcreatingconditioncompoundchannelabstractpolygon.h"

#include <guibase/vtktool/vtkpointsutil.h>

GridCreatingConditionCompoundChannel::EditPolygonCoordinatesCommand::EditPolygonCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GridCreatingConditionCompoundChannelAbstractPolygon* pol, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Edit Polygon Coordinates")},
	m_apply {apply},
	m_newCoords (coords),
	m_polygon {pol},
	m_cond {cond}
{
	m_oldCoords = vtkPointsUtil::getCoordinates(m_polygon->getVtkPolygon());
}

void GridCreatingConditionCompoundChannel::EditPolygonCoordinatesCommand::redo()
{
	vtkPointsUtil::setCoordinates(m_polygon->getVtkPolygon(), m_newCoords);
	m_polygon->updateShapeData();

	m_cond->renderGraphicsView();
}

void GridCreatingConditionCompoundChannel::EditPolygonCoordinatesCommand::undo()
{
	vtkPointsUtil::setCoordinates(m_polygon->getVtkPolygon(), m_oldCoords);
	m_polygon->updateShapeData();
	if (m_apply) {return;}

	m_cond->renderGraphicsView();
}
