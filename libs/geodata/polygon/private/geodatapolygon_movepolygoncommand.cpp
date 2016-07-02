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
	QVector2D fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QVector2D toVec(dx, dy);
	m_offset = toVec - fromVec;
	m_oldMapped = pol->isMapped();
	m_polygon = pol;
}

void GeoDataPolygon::MovePolygonCommand::redo()
{
	m_polygon->setMapped(false);
	m_polygon->m_shapeUpdating = true;
	movePolygon(m_polygon->m_gridRegionPolygon, m_offset);
	for (int i = 0; i < m_polygon->m_holePolygons.count(); ++i) {
		GeoDataPolygonHolePolygon* hp = m_polygon->m_holePolygons.at(i);
		movePolygon(hp, m_offset);
	}
	m_polygon->m_shapeUpdating = false;
	m_polygon->renderGraphicsView();
	m_polygon->updatePolyData();
}

void GeoDataPolygon::MovePolygonCommand::undo()
{
	m_polygon->setMapped(m_oldMapped);
	m_polygon->m_shapeUpdating = true;
	movePolygon(m_polygon->m_gridRegionPolygon, - m_offset);
	for (int i = 0; i < m_polygon->m_holePolygons.count(); ++i) {
		GeoDataPolygonHolePolygon* hp = m_polygon->m_holePolygons.at(i);
		movePolygon(hp, - m_offset);
	}
	m_polygon->m_shapeUpdating = false;
	m_polygon->renderGraphicsView();
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

void GeoDataPolygon::MovePolygonCommand::movePolygon(GeoDataPolygonAbstractPolygon* polygon, const QVector2D& offset)
{
	vtkPolygon* pol = polygon->getVtkPolygon();
	vtkPoints* points = pol->GetPoints();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		double p[3];
		points->GetPoint(i, p);
		p[0] += offset.x();
		p[1] += offset.y();
		points->SetPoint(i, p);
	}
	points->Modified();
	pol->Modified();
	polygon->updateShapeData();
}
