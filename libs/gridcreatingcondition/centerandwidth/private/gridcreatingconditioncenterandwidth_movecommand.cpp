#include "gridcreatingconditioncenterandwidth_impl.h"
#include "gridcreatingconditioncenterandwidth_movecommand.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

#include <vtkPolyData.h>

GridCreatingConditionCenterAndWidth::MoveCommand::MoveCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionCenterAndWidth* cond) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Move Center Line")},
	m_keyDown {keyDown},
	m_condition {cond}
{
	auto v = cond->graphicsView();
	auto fromVec = v->viewportToWorld(from);
	auto toVec = v->viewportToWorld(to);
	m_offset = toVec - fromVec;
}

void GridCreatingConditionCenterAndWidth::MoveCommand::redo()
{
	auto line = m_condition->polyLine();
	for (QPointF& p : line) {
		p += m_offset;
	}
	m_condition->setPolyLine(line);
	if (m_condition->impl->m_isGridCreated) {
		m_condition->createSpline(m_condition->impl->m_polyLineController.polyData()->GetPoints(), m_condition->impl->m_setting.iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::MoveCommand::undo()
{
	auto line = m_condition->polyLine();
	for (QPointF& p : line) {
		p -= m_offset;
	}
	m_condition->setPolyLine(line);
	if (m_condition->impl->m_isGridCreated) {
		m_condition->createSpline(m_condition->impl->m_polyLineController.polyData()->GetPoints(), m_condition->impl->m_setting.iMax - 1);
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
