#include "geodatapolygon_pushnewpointcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolygon::PushNewPointCommand::PushNewPointCommand(bool keyDown, const QPoint& point, GeoDataPolygon* pol) :
	GeoDataPolygon::ModifyAbstractPolygonCommand(pol->m_selectedPolygon, pol, GeoDataPolygon::tr("Add New Polygon Point")),
	m_keyDown {keyDown}
{
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);

	QPolygonF newPolygon(pol->m_selectedPolygon->polygon());
	if (keyDown) {
		newPolygon.insert(newPolygon.size() - 1, QPointF(dx, dy));
	} else {
		newPolygon[newPolygon.size() - 1] = QPointF(dx, dy);
	}
	setNewPolygon(newPolygon);
}

int GeoDataPolygon::PushNewPointCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPolygonPushNewPoint");
}

bool GeoDataPolygon::PushNewPointCommand::mergeWith(const QUndoCommand* other)
{
	const PushNewPointCommand* comm = dynamic_cast<const PushNewPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}

	return ModifyAbstractPolygonCommand::mergeWith(other);
}
