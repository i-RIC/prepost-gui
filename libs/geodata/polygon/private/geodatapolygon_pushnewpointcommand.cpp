#include "geodatapolygon_pushnewpointcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GeoDataPolygon::PushNewPointCommand::PushNewPointCommand(bool keyDown, const QPoint& point, GeoDataPolygon* pol) :
	GeoDataPolygon::ModifyAbstractPolygonCommand(pol->selectedPolygon(), pol, GeoDataPolygon::tr("Add New Polygon Point")),
	m_keyDown {keyDown}
{
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);

	QPointF newPoint(dx, dy);
	QPolygonF newPolygon = pol->selectedPolygon()->polygon();
	if (keyDown) {
		if (newPolygon.size() == 0) {
			newPolygon.push_back(newPoint);
			newPolygon.push_back(newPoint);
			newPolygon.push_back(newPoint);
		} else if (newPolygon.size() == 1) {
			newPolygon.push_back(newPoint);
			newPolygon.push_back(newPolygon.at(0));
		} else {
			newPolygon.insert(newPolygon.size() - 1, newPoint);
		}
	} else {
		newPolygon[newPolygon.size() - 2] = newPoint;
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

void GeoDataPolygon::PushNewPointCommand::undo()
{
	ModifyAbstractPolygonCommand::undo();
	if (m_targetPolygon->polygon().size() == 0) {
		m_polygon->setMouseEventMode(meBeforeDefining);
	}
}
