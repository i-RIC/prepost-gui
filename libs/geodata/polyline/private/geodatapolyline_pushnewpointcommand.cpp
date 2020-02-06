#include "geodatapolyline_pushnewpointcommand.h"
#include "../geodatapolylineimplpolyline.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GeoDataPolyLine::PushNewPointCommand::PushNewPointCommand(bool keyDown, const QPoint& point, GeoDataPolyLine* pol) :
	GeoDataPolyLine::ModifyAbstractPolyLineCommand(pol, GeoDataPolyLine::tr("Add New PolyLine Point")),
	m_keyDown {keyDown}
{
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);

	QPointF newPoint(dx, dy);
	auto newPolyLine = pol->polyLine()->polyLine();
	if (keyDown) {
		if (newPolyLine.size() == 0) {
			newPolyLine.push_back(newPoint);
			newPolyLine.push_back(newPoint);
		} else {
			newPolyLine.push_back(newPoint);
		}
	} else {
		newPolyLine[newPolyLine.size() - 1] = newPoint;
	}
	setNewPolyLine(newPolyLine);
}

int GeoDataPolyLine::PushNewPointCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPolyLinePushNewPoint");
}

bool GeoDataPolyLine::PushNewPointCommand::mergeWith(const QUndoCommand* other)
{
	const PushNewPointCommand* comm = dynamic_cast<const PushNewPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}

	return ModifyAbstractPolyLineCommand::mergeWith(other);
}

void GeoDataPolyLine::PushNewPointCommand::undo()
{
	ModifyAbstractPolyLineCommand::undo();
	if (m_targetPolyLine->polyLine().size() == 0) {
		m_polyline->setMouseEventMode(meBeforeDefining);
	}
}
