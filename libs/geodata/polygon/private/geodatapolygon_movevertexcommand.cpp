#include "geodatapolygon_movevertexcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

#include <QPointF>

GeoDataPolygon::MoveVertexCommand::MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, int vertexId, GeoDataPolygon* pol) :
	GeoDataPolygon::ModifyAbstractPolygonCommand(pol->selectedPolygon(), pol, GeoDataPolygon::tr("Move Polygon Vertex")),
	m_keyDown {keyDown}
{
	QPolygonF oldPolygon = (pol->selectedPolygon()->polygon());
	QPolygonF newPolygon = oldPolygon;

	double dx = from.x();
	double dy = from.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF toVec(dx, dy);

	auto offset = toVec - fromVec;

	auto oldP = oldPolygon.at(vertexId);
	auto newP = oldP + offset;

	newPolygon[vertexId] = newP;
	if (vertexId == 0) {
		newPolygon[newPolygon.size() - 1] = newP;
	}
	setNewPolygon(newPolygon);
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

	return ModifyAbstractPolygonCommand::mergeWith(other);
}
