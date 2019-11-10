#include "gridcreatingconditioncenterandwidth_removevertexcommand.h"

#include <vtkPolyData.h>

GridCreatingConditionCenterAndWidth::RemoveVertexCommand::RemoveVertexCommand(vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond) :
	PolyLineRemoveVertexCommand {GridCreatingConditionCenterAndWidth::tr("Remove Center Line Vertex"), vertexId, &(cond->m_polyLineController)},
	m_condition {cond}
{}

void GridCreatingConditionCenterAndWidth::RemoveVertexCommand::redo()
{
	PolyLineRemoveVertexCommand::redo();
	m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meNormal;
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::RemoveVertexCommand::undo()
{
	PolyLineRemoveVertexCommand::undo();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}
