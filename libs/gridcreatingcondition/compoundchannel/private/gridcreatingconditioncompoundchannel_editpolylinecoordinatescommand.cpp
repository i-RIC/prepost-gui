#include "gridcreatingconditioncompoundchannel_editpolylinecoordinatescommand.h"
#include "../gridcreatingconditioncompoundchannelabstractline.h"

#include <guibase/vtktool/vtkpointsutil.h>

GridCreatingConditionCompoundChannel::EditPolyLineCoordinatesCommand::EditPolyLineCoordinatesCommand(bool apply, const std::vector<QPointF>& coords, GridCreatingConditionCompoundChannelAbstractLine* l, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Edit Center Line Coordinates")},
	m_apply {apply},
	m_newCoords (coords),
	m_line {l},
	m_condition {cond}
{
	m_oldCoords = vtkPointsUtil::getCoordinates(m_line->getVtkLine());
}

void GridCreatingConditionCompoundChannel::EditPolyLineCoordinatesCommand::redo()
{
	vtkPointsUtil::setCoordinates(m_line->getVtkLine(), m_newCoords);
	m_line->updateShapeData();

	m_condition->renderGraphicsView();
}

void GridCreatingConditionCompoundChannel::EditPolyLineCoordinatesCommand::undo()
{
	vtkPointsUtil::setCoordinates(m_line->getVtkLine(), m_oldCoords);
	m_line->updateShapeData();
	if (m_apply) {return;}

	m_condition->renderGraphicsView();
}
