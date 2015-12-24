#include "gridcreatingconditioncenterandwidth_removevertexcommand.h"

GridCreatingConditionCenterAndWidth::RemoveVertexCommand::RemoveVertexCommand(vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Remove Center Line Vertex")},
	m_vertexId {vertexId},
	m_condition {cond}
{
	double p[3];
	cond->m_vtkPolyLine->GetPoints()->GetPoint(m_vertexId, p);
	m_vertexPosition = QVector2D(p[0], p[1]);
}

void GridCreatingConditionCenterAndWidth::RemoveVertexCommand::redo()
{
	vtkPoints* points = m_condition->m_vtkPolyLine->GetPoints();
	QVector<QVector2D> positions;
	positions.reserve(points->GetNumberOfPoints());
	double p[3];
	for (vtkIdType i = 0; i < m_vertexId; ++i) {
		points->GetPoint(i, p);
		positions.append(QVector2D(p[0], p[1]));
	}
	// skip vertex in m_vertexId
	for (vtkIdType i = m_vertexId + 1; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, p);
		positions.append(QVector2D(p[0], p[1]));
	}
	points->SetNumberOfPoints(positions.count());
	for (vtkIdType i = 0; i < positions.count(); ++i) {
		QVector2D v = positions.at(i);
		points->SetPoint(i, v.x(), v.y(), 0);
	}
	points->Modified();
	m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meNormal;
	m_condition->m_vtkPolyLine->Modified();
	m_condition->updateShapeData();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::RemoveVertexCommand::undo()
{
	vtkPoints* points = m_condition->m_vtkPolyLine->GetPoints();
	QVector<QVector2D> positions;
	positions.reserve(points->GetNumberOfPoints());
	double p[3];
	for (vtkIdType i = 0; i < m_vertexId; ++i) {
		points->GetPoint(i, p);
		positions.append(QVector2D(p[0], p[1]));
	}
	positions.append(m_vertexPosition);
	for (vtkIdType i = m_vertexId; i < points->GetNumberOfPoints(); ++i) {
		points->GetPoint(i, p);
		positions.append(QVector2D(p[0], p[1]));
	}
	points->SetNumberOfPoints(positions.count());
	for (vtkIdType i = 0; i < positions.count(); ++i) {
		QVector2D v = positions.at(i);
		points->SetPoint(i, v.x(), v.y(), 0);
	}
	points->Modified();
	m_condition->m_vtkPolyLine->Modified();
	m_condition->updateShapeData();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->gridCreated(m_condition->createGrid());
	}
}
