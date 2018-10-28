#include "geodatapointmap_addpointcommand.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPointmap::AddPointCommand::AddPointCommand(bool keyDown, const QPoint& point, GeoDataPointmap* ptmap) :
	QUndoCommand {GeoDataPointmap::tr("Add New Selection Polygon Point")}
{
	m_keyDown = keyDown;
	double dx = point.x();
	double dy = point.y();
	ptmap->graphicsView()->viewportToWorld(dx, dy);
	m_newPoint = QPointF(dx, dy);
	m_pointMap = ptmap;
}

void GeoDataPointmap::AddPointCommand::redo()
{
	vtkPolygon* pol = m_pointMap->m_vtkPolygon;
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
	m_pointMap->updateShapeData();
}

void GeoDataPointmap::AddPointCommand::undo()
{
	vtkPolygon* pol = m_pointMap->m_vtkPolygon;
	if (m_keyDown) {
		// decrease the number of points. i. e. remove the last point.
		vtkIdType numOfPoints = pol->GetPoints()->GetNumberOfPoints();
		pol->GetPoints()->SetNumberOfPoints(numOfPoints - 1);
		pol->GetPoints()->Modified();
	} else {
		// this does not happen. no implementation needed.
	}
	pol->Modified();
	m_pointMap->updateShapeData();
}

int GeoDataPointmap::AddPointCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPMPolygonAddPoint");
}

bool GeoDataPointmap::AddPointCommand::mergeWith(const QUndoCommand* other)
{
	const AddPointCommand* comm = dynamic_cast<const AddPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_pointMap != m_pointMap) {return false;}
	m_newPoint = comm->m_newPoint;
	return true;
}
