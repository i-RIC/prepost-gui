#include "gridcreatingconditiontriangle_removepolylinevertexcommand.h"
#include "../gridcreatingconditiontriangleabstractline.h"

GridCreatingConditionTriangle::RemovePolyLineVertexCommand::RemovePolyLineVertexCommand(vtkIdType vertexId, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Remove Break Line Vertex")},
	m_vertexId {vertexId},
	m_polygon {pol},
	m_targetLine {pol->m_selectedLine}
{
	double p[3];
	pol->m_selectedLine->getVtkLine()->GetPoints()->GetPoint(m_vertexId, p);
	m_vertexPosition = QVector2D(p[0], p[1]);
}

void GridCreatingConditionTriangle::RemovePolyLineVertexCommand::redo()
{
	vtkPoints* points = m_targetLine->getVtkLine()->GetPoints();
	QVector<QVector2D> positions;
	positions.reserve(points->GetNumberOfPoints());
	double p[3];
	for (vtkIdType i = 0; i < m_vertexId; ++i) {
		points->GetPoint(i, p);
		positions.append(QVector2D(p[0], p[1]));
	}
	// skip vertex in m_vertexId[
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
	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
	m_targetLine->getVtkLine()->Modified();
	m_targetLine->updateShapeData();
	m_polygon->renderGraphicsView();
}

void GridCreatingConditionTriangle::RemovePolyLineVertexCommand::undo()
{
	vtkPoints* points = m_targetLine->getVtkLine()->GetPoints();
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
	m_targetLine->getVtkLine()->Modified();
	m_targetLine->updateShapeData();
	m_polygon->renderGraphicsView();
}
