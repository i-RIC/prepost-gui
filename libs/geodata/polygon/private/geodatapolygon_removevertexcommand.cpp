#include "geodatapolygon_removevertexcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolygon::RemoveVertexCommand::RemoveVertexCommand(vtkIdType vertexId, GeoDataPolygon* pol) :
	QUndoCommand {GeoDataPolygon::tr("Remove Polygon Vertex")}
{
	m_vertexId = vertexId;
	double p[3];
	pol->m_selectedPolygon->getVtkPolygon()->GetPoints()->GetPoint(m_vertexId, p);
	m_vertexPosition = QVector2D(p[0], p[1]);
	m_oldMapped = pol->isMapped();
	m_polygon = pol;
	m_targetPolygon = m_polygon->m_selectedPolygon;
}

void GeoDataPolygon::RemoveVertexCommand::redo()
{
	m_polygon->setMapped(false);
	m_polygon->m_shapeUpdating = true;
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
	m_polygon->m_mouseEventMode = GeoDataPolygon::meNormal;
	m_targetPolygon->getVtkPolygon()->Modified();
	m_targetPolygon->updateShapeData();
	m_polygon->m_shapeUpdating = false;
	m_polygon->renderGraphicsView();
	m_polygon->updatePolyData();
}

void GeoDataPolygon::RemoveVertexCommand::undo()
{
	m_polygon->setMapped(m_oldMapped);
	m_polygon->m_shapeUpdating = false;
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
	m_targetPolygon->getVtkPolygon()->Modified();
	m_targetPolygon->updateShapeData();
	m_polygon->m_shapeUpdating = false;
	m_polygon->renderGraphicsView();
	m_polygon->updatePolyData();
}
