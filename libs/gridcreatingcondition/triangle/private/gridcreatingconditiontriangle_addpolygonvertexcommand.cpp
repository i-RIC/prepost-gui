#include "gridcreatingconditiontriangle_addpolygonvertexcommand.h"
#include "../gridcreatingconditiontriangleabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionTriangle::AddPolygonVertexCommand::AddPolygonVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionTriangle* pol) :
	QUndoCommand(GridCreatingConditionTriangle::tr("Insert Polygon Vertex")),
	m_keyDown {keyDown},
	m_vertexId {(edgeId + 1) % (pol->m_selectedPolygon->getVtkPolygon()->GetNumberOfPoints() + 1)},
	m_polygon {pol},
	m_targetPolygon {pol->m_selectedPolygon}
{
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	m_vertexPosition = QVector2D(dx, dy);
}

void GridCreatingConditionTriangle::AddPolygonVertexCommand::redo()
{
	if (m_keyDown) {
		// add vertex.
		vtkPoints* points = m_targetPolygon->getVtkPolygon()->GetPoints();
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
	} else {
		// just modify the vertex position
		vtkPoints* points = m_targetPolygon->getVtkPolygon()->GetPoints();
		points->SetPoint(m_vertexId, m_vertexPosition.x(), m_vertexPosition.y(), 0);
		points->Modified();
	}
	m_targetPolygon->getVtkPolygon()->Modified();
	m_targetPolygon->updateShapeData();
	m_polygon->renderGraphicsView();
}

void GridCreatingConditionTriangle::AddPolygonVertexCommand::undo()
{
	if (m_keyDown) {
		// remove vertex.
		vtkPoints* points = m_targetPolygon->getVtkPolygon()->GetPoints();
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
		m_targetPolygon->getVtkPolygon()->Modified();
		m_targetPolygon->updateShapeData();
		m_polygon->renderGraphicsView();
	} else {
		// this never happens.
	}
}

int GridCreatingConditionTriangle::AddPolygonVertexCommand::id() const
{
	return iRIC::generateCommandId("AddPolygonVertex");
}

bool GridCreatingConditionTriangle::AddPolygonVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddPolygonVertexCommand* comm = dynamic_cast<const AddPolygonVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_polygon != comm->m_polygon) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}
	m_vertexPosition = comm->m_vertexPosition;
	return true;
}
