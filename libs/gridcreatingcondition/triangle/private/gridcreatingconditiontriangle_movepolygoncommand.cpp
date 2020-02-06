#include "gridcreatingconditiontriangle_movepolygoncommand.h"
#include "../gridcreatingconditiontriangleabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionTriangle::MovePolygonCommand::MovePolygonCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Move Polygon")},
	m_keyDown {keyDown},
	m_polygon {pol},
	m_targetPolygon {pol->m_selectedPolygon}
{
	double dx = from.x();
	double dy = from.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QVector2D fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QVector2D toVec(dx, dy);
	m_offset = toVec - fromVec;
}

void GridCreatingConditionTriangle::MovePolygonCommand::redo()
{
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	vtkPoints* points = pol->GetPoints();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		double p[3];
		points->GetPoint(i, p);
		p[0] += m_offset.x();
		p[1] += m_offset.y();
		points->SetPoint(i, p);
	}
	points->Modified();
	pol->Modified();
	m_targetPolygon->updateShapeData();
	m_polygon->renderGraphicsView();
}

void GridCreatingConditionTriangle::MovePolygonCommand::undo()
{
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	vtkPoints* points = pol->GetPoints();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		double p[3];
		points->GetPoint(i, p);
		p[0] -= m_offset.x();
		p[1] -= m_offset.y();
		points->SetPoint(i, p);
	}
	points->Modified();
	pol->Modified();
	m_targetPolygon->updateShapeData();
	m_polygon->renderGraphicsView();
}

int GridCreatingConditionTriangle::MovePolygonCommand::id() const
{
	return iRIC::generateCommandId("MovePolygon");
}

bool GridCreatingConditionTriangle::MovePolygonCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolygonCommand* comm = dynamic_cast<const MovePolygonCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polygon != m_polygon) {return false;}
	if (comm->m_targetPolygon != m_targetPolygon) {return false;}
	m_offset += comm->m_offset;
	return true;
}
