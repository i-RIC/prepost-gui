#include "geodatapolygon_movevertexcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

#include <QPointF>

GeoDataPolygon::MoveVertexCommand::MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GeoDataPolygon* pol) :
	QUndoCommand {GeoDataPolygon::tr("Move Polygon Vertex")},
	m_keyDown {keyDown},
	m_newPolygon {pol->m_selectedPolygon->polygon()},
	m_oldPolygon {pol->m_selectedPolygon->polygon()},
	m_polygon {pol},
	m_targetPolygon {pol->m_selectedPolygon},
	m_oldMapped {pol->isMapped()}
{
	double dx = from.x();
	double dy = from.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF toVec(dx, dy);

	auto offset = toVec - fromVec;

	auto oldP = m_oldPolygon.at(vertexId);
	auto newP = oldP + offset;

	m_newPolygon[vertexId] = newP;
	if (vertexId == 0) {
		m_newPolygon[m_newPolygon.size() - 1] = newP;
	}
}

void GeoDataPolygon::MoveVertexCommand::redo()
{
	m_polygon->setMapped(false);
	m_targetPolygon->setPolygon(m_newPolygon);
	m_polygon->renderGraphicsView();
	m_polygon->updatePolyData();
}

void GeoDataPolygon::MoveVertexCommand::undo()
{
	m_polygon->setMapped(m_oldMapped);
	m_targetPolygon->setPolygon(m_oldPolygon);
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
	m_newPolygon = comm->m_newPolygon;
	return true;
}
