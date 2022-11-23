#include "gridcreatingconditioncenterandwidth_impl.h"
#include "gridcreatingconditioncenterandwidth_removevertexcommand.h"

#include <vtkPolyData.h>

GridCreatingConditionCenterAndWidth::RemoveVertexCommand::RemoveVertexCommand(vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond) :
	PolyLineRemoveVertexCommand {GridCreatingConditionCenterAndWidth::tr("Remove Center Line Vertex"), static_cast<int> (vertexId), &(cond->impl->m_polyLineController)},
	m_condition {cond}
{}

void GridCreatingConditionCenterAndWidth::RemoveVertexCommand::redo()
{
	PolyLineRemoveVertexCommand::redo();
	m_condition->impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
	if (m_condition->impl->m_isGridCreated) {
		m_condition->createSpline(m_condition->impl->m_polyLineController.polyData()->GetPoints(), m_condition->impl->m_setting.iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::RemoveVertexCommand::undo()
{
	PolyLineRemoveVertexCommand::undo();
	if (m_condition->impl->m_isGridCreated) {
		m_condition->createSpline(m_condition->impl->m_polyLineController.polyData()->GetPoints(), m_condition->impl->m_setting.iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}
