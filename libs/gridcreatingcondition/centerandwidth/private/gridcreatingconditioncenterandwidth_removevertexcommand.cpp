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
	vtkPoints* points = m_condition->m_polyLineController.polyData()->GetPoints();
	std::vector<QPointF> positions;
	positions.reserve(points->GetNumberOfPoints());
	double p[3];
	for (vtkIdType i = 0; i < m_vertexId; ++i) {
		points->GetPoint(i, p);
		positions.push_back(QPointF(p[0], p[1]));
	}
	// skip vertex in m_vertexId
	for (vtkIdType i = m_vertexId + 1; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, p);
		positions.push_back(QPointF(p[0], p[1]));
	}
	points->SetNumberOfPoints(positions.size());
	for (vtkIdType i = 0; i < positions.size(); ++i) {
		QPointF v = positions.at(i);
		points->SetPoint(i, v.x(), v.y(), 0);
	}
	points->Modified();
	m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meNormal;
	m_condition->m_polyLineController.polyData()->Modified();
	m_condition->updateShapeData();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::RemoveVertexCommand::undo()
{
	vtkPoints* points = m_condition->m_polyLineController.polyData()->GetPoints();
	std::vector<QPointF> positions;
	positions.reserve(points->GetNumberOfPoints());
	double p[3];
	for (vtkIdType i = 0; i < m_vertexId; ++i) {
		points->GetPoint(i, p);
		positions.push_back(QPointF(p[0], p[1]));
	}
	positions.push_back(m_vertexPosition);
	for (vtkIdType i = m_vertexId; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, p);
		positions.push_back(QPointF(p[0], p[1]));
	}
	points->SetNumberOfPoints(positions.size());
	for (vtkIdType i = 0; i < positions.size(); ++i) {
		QPointF v = positions.at(i);
		points->SetPoint(i, v.x(), v.y(), 0);
	}
	points->Modified();
	m_condition->m_polyLineController.polyData()->Modified();
	m_condition->updateShapeData();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}
