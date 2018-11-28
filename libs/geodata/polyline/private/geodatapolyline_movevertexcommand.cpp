#include "geodatapolyline_movevertexcommand.h"
#include "../geodatapolylineimplpolyline.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

#include <QPointF>

GeoDataPolyLine::MoveVertexCommand::MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, int vertexId, GeoDataPolyLine* pol) :
	GeoDataPolyLine::ModifyAbstractPolyLineCommand(pol, GeoDataPolyLine::tr("Move PolyLine Vertex")),
	m_keyDown {keyDown}
{
	auto oldPolyLine = (pol->polyLine()->polyLine());
	auto newPolyLine = oldPolyLine;

	double dx = from.x();
	double dy = from.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF toVec(dx, dy);

	auto offset = toVec - fromVec;

	auto oldP = oldPolyLine.at(vertexId);
	auto newP = oldP + offset;

	newPolyLine[vertexId] = newP;
	setNewPolyLine(newPolyLine);
}

int GeoDataPolyLine::MoveVertexCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPolyLinePolyLineMoveVertex");
}

bool GeoDataPolyLine::MoveVertexCommand::mergeWith(const QUndoCommand* other)
{
	const MoveVertexCommand* comm = dynamic_cast<const MoveVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}

	return ModifyAbstractPolyLineCommand::mergeWith(other);
}
