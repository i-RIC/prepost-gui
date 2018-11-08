#include "gridcreatingconditioncompoundchannel_movepolylinecommand.h"
#include "../gridcreatingconditioncompoundchannelabstractline.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionCompoundChannel::MovePolyLineCommand::MovePolyLineCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Move Polygonal Line")},
	m_keyDown {keyDown},
	m_targetLine {cond->m_selectedLine},
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

void GridCreatingConditionCompoundChannel::MovePolyLineCommand::redo()
{
	applyOffset(m_offset);
}

void GridCreatingConditionCompoundChannel::MovePolyLineCommand::undo()
{
	applyOffset(-m_offset);
}

int GridCreatingConditionCompoundChannel::MovePolyLineCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCompoundChannelMovePolyLine");
}

bool GridCreatingConditionCompoundChannel::MovePolyLineCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolyLineCommand* comm = dynamic_cast<const MovePolyLineCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_targetLine != m_targetLine) {return false;}
	if (comm->m_condition != m_condition) {return false;}
	m_offset += comm->m_offset;
	return true;
}

void GridCreatingConditionCompoundChannel::MovePolyLineCommand::applyOffset(const QPointF& offset)
{
	vtkPolyLine* pol = m_targetLine->getVtkLine();
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
	m_targetLine->updateShapeData();
}
