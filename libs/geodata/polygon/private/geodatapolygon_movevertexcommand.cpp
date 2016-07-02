#include "geodatapolygon_movevertexcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolygon::MoveVertexCommand::MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GeoDataPolygon* pol) :
	QUndoCommand {GeoDataPolygon::tr("Move Polygon Vertex")}
{
	m_keyDown = keyDown;
	m_vertexId = vertexId;
	double dx = from.x();
	double dy = from.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QVector2D fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QVector2D toVec(dx, dy);
	m_offset = toVec - fromVec;
	m_oldMapped = pol->isMapped();
	m_polygon = pol;
	m_targetPolygon = m_polygon->m_selectedPolygon;
}

void GeoDataPolygon::MoveVertexCommand::redo()
{
	m_polygon->setMapped(false);
	m_polygon->m_shapeUpdating = true;
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	vtkPoints* points = pol->GetPoints();
	double p[3];
	points->GetPoint(m_vertexId, p);
	p[0] += m_offset.x();
	p[1] += m_offset.y();
	points->SetPoint(m_vertexId, p);

	points->Modified();
	pol->Modified();
	m_targetPolygon->updateShapeData();
	m_polygon->m_shapeUpdating = false;
	m_polygon->renderGraphicsView();
	m_polygon->updatePolyData();
}

void GeoDataPolygon::MoveVertexCommand::undo()
{
	m_polygon->setMapped(m_oldMapped);
	m_polygon->m_shapeUpdating = true;
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	vtkPoints* points = pol->GetPoints();
	double p[3];
	points->GetPoint(m_vertexId, p);
	p[0] -= m_offset.x();
	p[1] -= m_offset.y();
	points->SetPoint(m_vertexId, p);

	points->Modified();
	pol->Modified();
	m_targetPolygon->updateShapeData();
	m_polygon->m_shapeUpdating = false;
	m_polygon->renderGraphicsView();
	m_polygon->updatePolyData();
}

int GeoDataPolygon::MoveVertexCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPolygonPolygonMoveVertex");
}

bool GeoDataPolygon::MoveVertexCommand::mergeWith(const QUndoCommand* other)
{
	const MoveVertexCommand* comm = dynamic_cast<const MoveVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polygon != m_polygon) {return false;}
	if (comm->m_targetPolygon != m_targetPolygon) {return false;}
	if (comm->m_vertexId != m_vertexId) {return false;}
	m_offset += comm->m_offset;
	return true;
}
