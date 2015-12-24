#include "gridcreatingconditioncenterandwidth_movecommand.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionCenterAndWidth::MoveCommand::MoveCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionCenterAndWidth* cond) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Move Center Line")},
	m_keyDown {keyDown},
	m_condition {cond}
{
	double dx = from.x();
	double dy = from.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QVector2D fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QVector2D toVec(dx, dy);
	m_offset = toVec - fromVec;
}

void GridCreatingConditionCenterAndWidth::MoveCommand::redo()
{
	vtkPolyLine* line =  m_condition->m_vtkPolyLine;
	vtkPoints* points = line->GetPoints();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		double p[3];
		points->GetPoint(i, p);
		p[0] += m_offset.x();
		p[1] += m_offset.y();
		points->SetPoint(i, p);
	}
	points->Modified();
	line->Modified();
	m_condition->updateShapeData();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
	m_condition->setModified();
}

void GridCreatingConditionCenterAndWidth::MoveCommand::undo()
{
	vtkPolyLine* line =  m_condition->m_vtkPolyLine;
	vtkPoints* points = line->GetPoints();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		double p[3];
		points->GetPoint(i, p);
		p[0] -= m_offset.x();
		p[1] -= m_offset.y();
		points->SetPoint(i, p);
	}
	points->Modified();
	line->Modified();
	m_condition->updateShapeData();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

int GridCreatingConditionCenterAndWidth::MoveCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthMove");
}

bool GridCreatingConditionCenterAndWidth::MoveCommand::mergeWith(const QUndoCommand* other)
{
	const MoveCommand* comm = dynamic_cast<const MoveCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_condition != m_condition) {return false;}

	m_offset += comm->m_offset;
	return true;
}
