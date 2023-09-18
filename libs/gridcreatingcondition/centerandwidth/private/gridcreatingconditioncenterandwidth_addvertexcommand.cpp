#include "gridcreatingconditioncenterandwidth_addvertexcommand.h"
#include "gridcreatingconditioncenterandwidth_impl.h"

#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <misc/qundocommandhelper.h>

#include <vtkPolyData.h>

GridCreatingConditionCenterAndWidth::AddVertexCommand::AddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCenterAndWidth* cond) :
	PolyLineAddVertexCommand {GridCreatingConditionCenterAndWidth::tr("Insert Center Line Vertex"), keyDown, static_cast<int> (edgeId), cond->graphicsView()->viewportToWorld(point),
														&(cond->impl->m_polyLineController)},
	m_condition {cond}
{}

void GridCreatingConditionCenterAndWidth::AddVertexCommand::redo()
{
	PolyLineAddVertexCommand::redo();
	if (m_condition->impl->m_isGridCreated) {
		m_condition->createSpline(m_condition->impl->m_polyLineController.polyData()->GetPoints(), m_condition->impl->m_setting.iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::AddVertexCommand::undo()
{
	PolyLineAddVertexCommand::undo();
	if (m_condition->impl->m_isGridCreated) {
		m_condition->createSpline(m_condition->impl->m_polyLineController.polyData()->GetPoints(), m_condition->impl->m_setting.iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

int GridCreatingConditionCenterAndWidth::AddVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthAddVertex");
}

bool GridCreatingConditionCenterAndWidth::AddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddVertexCommand* comm = dynamic_cast<const AddVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (m_condition != comm->m_condition) {return false;}
	return PolyLineAddVertexCommand::mergeWith(other);
}
