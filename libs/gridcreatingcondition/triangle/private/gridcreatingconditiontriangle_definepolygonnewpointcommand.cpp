#include "gridcreatingconditiontriangle_definepolygonnewpointcommand.h"
#include "../gridcreatingconditiontriangleabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionTriangle::DefinePolygonNewPointCommand::DefinePolygonNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Add New Polygon Point")},
	m_keyDown {keyDown},
	m_polygon {pol},
	m_targetPolygon {pol->m_selectedPolygon}
{
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	m_newPoint = QVector2D(dx, dy);
}

void GridCreatingConditionTriangle::DefinePolygonNewPointCommand::redo()
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
	m_targetPolygon->updateShapeData();
	m_polygon->renderGraphicsView();
}

void GridCreatingConditionTriangle::DefinePolygonNewPointCommand::undo()
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
	m_targetPolygon->updateShapeData();
	m_polygon->renderGraphicsView();
}

int GridCreatingConditionTriangle::DefinePolygonNewPointCommand::id() const
{
	return iRIC::generateCommandId("DefinePolygonNewPoint");
}

bool GridCreatingConditionTriangle::DefinePolygonNewPointCommand::mergeWith(const QUndoCommand* other)
{
	const DefinePolygonNewPointCommand* comm = dynamic_cast<const DefinePolygonNewPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polygon != m_polygon) {return false;}
	if (comm->m_targetPolygon != m_targetPolygon) {return false;}
	m_newPoint = comm->m_newPoint;
	return true;
}
