#include "gridcreatingconditiontriangle_addpolylinevertexcommand.h"
#include "../gridcreatingconditiontriangleabstractline.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionTriangle::AddPolyLineVertexCommand::AddPolyLineVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Insert Break Line Vertex")},
	m_keyDown {keyDown},
	m_vertexId {edgeId + 1},
	m_polygon {pol},
	m_targetLine {pol->m_selectedLine}
{
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	m_vertexPosition = QVector2D(dx, dy);
}

void GridCreatingConditionTriangle::AddPolyLineVertexCommand::redo()
{
	if (m_keyDown) {
		// add vertex.
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
	} else {
		// just modify the vertex position
		vtkPoints* points = m_targetLine->getVtkLine()->GetPoints();
		points->SetPoint(m_vertexId, m_vertexPosition.x(), m_vertexPosition.y(), 0);
		points->Modified();
	}
	m_targetLine->getVtkLine()->Modified();
	m_targetLine->updateShapeData();
	m_polygon->renderGraphicsView();
}

void GridCreatingConditionTriangle::AddPolyLineVertexCommand::undo()
{
	if (m_keyDown) {
		// remove vertex.
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
		m_targetLine->getVtkLine()->Modified();
		m_targetLine->updateShapeData();
		m_polygon->renderGraphicsView();
	} else {
		// this never happens.
	}
}

int GridCreatingConditionTriangle::AddPolyLineVertexCommand::id() const
{
	return iRIC::generateCommandId("PolyLineAddVertex");
}

bool GridCreatingConditionTriangle::AddPolyLineVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddPolyLineVertexCommand* comm = dynamic_cast<const AddPolyLineVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_polygon != comm->m_polygon) {return false;}
	if (m_targetLine != comm->m_targetLine) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}
	m_vertexPosition = comm->m_vertexPosition;
	return true;
}
