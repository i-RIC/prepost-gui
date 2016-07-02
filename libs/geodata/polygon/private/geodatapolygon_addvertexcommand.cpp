#include "geodatapolygon_addvertexcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolygon::AddVertexCommand::AddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GeoDataPolygon* pol) :
	QUndoCommand {GeoDataPolygon::tr("Insert Polygon Vertex")}
{
	m_keyDown = keyDown;
	m_vertexId = (edgeId + 1) % (pol->m_selectedPolygon->getVtkPolygon()->GetNumberOfPoints() + 1);
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	m_vertexPosition = QVector2D(dx, dy);
	m_oldMapped = pol->isMapped();
	m_polygon = pol;
	m_targetPolygon = m_polygon->m_selectedPolygon;
}

void GeoDataPolygon::AddVertexCommand::redo()
{
	m_polygon->setMapped(false);
	m_polygon->m_shapeUpdating = true;
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
	m_polygon->m_shapeUpdating = false;
	m_polygon->updatePolyData();
}

void GeoDataPolygon::AddVertexCommand::undo()
{
	m_polygon->setMapped(m_oldMapped);
	if (m_keyDown) {
		m_polygon->m_shapeUpdating = true;
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
		m_polygon->m_shapeUpdating = false;
		m_polygon->updatePolyData();
	} else {
		// this never happens.
	}
}

int GeoDataPolygon::AddVertexCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPolygonPolygonAddVertex");
}

bool GeoDataPolygon::AddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddVertexCommand* comm = dynamic_cast<const AddVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_polygon != comm->m_polygon) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}
	m_vertexPosition = comm->m_vertexPosition;
	return true;
}
