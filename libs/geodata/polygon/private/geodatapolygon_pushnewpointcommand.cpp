#include "geodatapolygon_pushnewpointcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolygon::PushNewPointCommand::PushNewPointCommand(bool keyDown, const QPoint& point, GeoDataPolygon* pol) :
	QUndoCommand {GeoDataPolygon::tr("Add New Polygon Point")}
{
	m_keyDown = keyDown;
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	m_newPoint = QVector2D(dx, dy);
	m_polygon = pol;
	m_oldMapped = m_polygon->isMapped();
	m_targetPolygon = m_polygon->m_selectedPolygon;
}

void GeoDataPolygon::PushNewPointCommand::redo()
{
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	if (m_keyDown) {
		// add new point.
		pol->GetPoints()->InsertNextPoint(m_newPoint.x(), m_newPoint.y(), 0);
		pol->GetPoints()->Modified();
	} else {
		// modify the last point.
		vtkIdType lastId = pol->GetNumberOfPoints() - 1;
		pol->GetPoints()->SetPoint(lastId, m_newPoint.x(), m_newPoint.y(), 0);
		pol->GetPoints()->Modified();
	}
	pol->Modified();
	m_polygon->setMapped(false);
	m_polygon->m_shapeUpdating = true;
	m_targetPolygon->updateShapeData();
	m_polygon->m_shapeUpdating = false;
	m_polygon->updatePolyData();
}

void GeoDataPolygon::PushNewPointCommand::undo()
{
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	if (m_keyDown) {
		// decrease the number of points. i. e. remove the last point.
		vtkIdType numOfPoints = pol->GetPoints()->GetNumberOfPoints();
		pol->GetPoints()->SetNumberOfPoints(numOfPoints - 1);
		pol->GetPoints()->Modified();
	} else {
		// this does not happen. no implementation needed.
	}
	pol->Modified();
	m_polygon->setMapped(m_oldMapped);
	m_polygon->m_shapeUpdating = true;
	m_targetPolygon->updateShapeData();
	m_polygon->m_shapeUpdating = false;
	m_polygon->updatePolyData();
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
	if (comm->m_polygon != m_polygon) {return false;}
	if (comm->m_targetPolygon != m_targetPolygon) {return false;}
	m_newPoint = comm->m_newPoint;
	return true;
}
