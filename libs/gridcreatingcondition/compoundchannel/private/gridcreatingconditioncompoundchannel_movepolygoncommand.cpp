#include "gridcreatingconditioncompoundchannel_movepolygoncommand.h"
#include "../gridcreatingconditioncompoundchannelabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionCompoundChannel::MovePolygonCommand::MovePolygonCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Move Polygon")},
	m_keyDown {keyDown},
	m_targetPolygon {cond->m_selectedPolygon},
	m_condition {cond}
{
	double dx = from.x();
	double dy = from.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QPointF fromVec(dx, dy);

	dx = to.x();
	dy = to.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QPointF toVec(dx, dy);

	m_offset = toVec - fromVec;
}

void GridCreatingConditionCompoundChannel::MovePolygonCommand::redo()
{
	applyOffset(m_offset);
}

void GridCreatingConditionCompoundChannel::MovePolygonCommand::undo()
{
	applyOffset(- m_offset);
}

void GridCreatingConditionCompoundChannel::MovePolygonCommand::applyOffset(const QPointF& offset)
{
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	vtkPoints* points = pol->GetPoints();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		double p[3];
		points->GetPoint(i, p);
		p[0] += offset.x();
		p[1] += offset.y();
		points->SetPoint(i, p);
	}
	points->Modified();
	pol->Modified();
	m_targetPolygon->updateShapeData();
}

int GridCreatingConditionCompoundChannel::MovePolygonCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCompoundChannelMovePolygonCommand");
}

bool GridCreatingConditionCompoundChannel::MovePolygonCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolygonCommand* comm = dynamic_cast<const MovePolygonCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_targetPolygon != m_targetPolygon) {return false;}
	if (comm->m_condition != m_condition) {return false;}
	m_offset += comm->m_offset;
	return true;
}
