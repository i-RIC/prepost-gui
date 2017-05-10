#include "gridcreatingconditioncenterandwidth_removevertexcommand.h"

#include <vtkPolyData.h>

GridCreatingConditionCenterAndWidth::RemoveVertexCommand::RemoveVertexCommand(vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Remove Center Line Vertex")},
	m_vertexId {vertexId},
	m_condition {cond}
{
	double p[3];
	cond->m_polyLineController.polyData()->GetPoints()->GetPoint(m_vertexId, p);
	m_vertexPosition = QPointF(p[0], p[1]);
}

void GridCreatingConditionCenterAndWidth::RemoveVertexCommand::redo()
{
	auto line = m_condition->polyLine();
	auto it = line.begin() + m_vertexId;
	line.erase(it);
	m_condition->setPolyLine(line);
	m_condition->updateShapeData();

	m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meNormal;

	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::RemoveVertexCommand::undo()
{
	auto line = m_condition->polyLine();
	auto it = line.begin() + m_vertexId;
	line.insert(it, m_vertexPosition);
	m_condition->setPolyLine(line);
	m_condition->updateShapeData();

	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}
