#include "geodatapolygon_movepolygoncommand.h"
#include "../geodatapolygonholepolygon.h"
#include "../geodatapolygonregionpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolygon::MovePolygonCommand::MovePolygonCommand(bool keyDown, const QPoint& from, const QPoint& to, GeoDataPolygon* pol) :
	QUndoCommand {GeoDataPolygon::tr("Move Polygon")}
{
	m_keyDown = keyDown;
	double dx = from.x();
	double dy = from.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF toVec(dx, dy);
	m_offset = toVec - fromVec;
	m_oldMapped = pol->isMapped();
	m_polygon = pol;
}

void GeoDataPolygon::MovePolygonCommand::redo()
{
	m_polygon->setMapped(false);
	m_polygon->setShapeUpdating(true);
	movePolygon(m_polygon->regionPolygon(), m_offset);
	for (int i = 0; i < m_polygon->holePolygons().size(); ++i) {
		GeoDataPolygonHolePolygon* hp = m_polygon->holePolygons().at(i);
		movePolygon(hp, m_offset);
	}
	m_polygon->setShapeUpdating(false);
	m_polygon->updatePolyData();
}

void GeoDataPolygon::MovePolygonCommand::undo()
{
	m_polygon->setMapped(m_oldMapped);
	m_polygon->setShapeUpdating(true);
	movePolygon(m_polygon->regionPolygon(), - m_offset);
	for (int i = 0; i < m_polygon->holePolygons().size(); ++i) {
		GeoDataPolygonHolePolygon* hp = m_polygon->holePolygons().at(i);
		movePolygon(hp, - m_offset);
	}
	m_polygon->setShapeUpdating(false);
	m_polygon->updatePolyData();
}

int GeoDataPolygon::MovePolygonCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPolygonPolygonMove");
}

bool GeoDataPolygon::MovePolygonCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolygonCommand* comm = dynamic_cast<const MovePolygonCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polygon != m_polygon) {return false;}
	m_offset += comm->m_offset;
	return true;
}

void GeoDataPolygon::MovePolygonCommand::movePolygon(GeoDataPolygonAbstractPolygon* polygon, const QPointF& offset)
{
	auto newPol = polygon->polygon(offset);
	polygon->setPolygon(newPol);
}
