#include "gridcreatingconditiontriangle_movepolylinecommand.h"
#include "../gridcreatingconditiontriangleabstractline.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionTriangle::MovePolyLineCommand::MovePolyLineCommand(bool keyDown, const QPoint& from, const QPoint& to, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Move Break Line")},
	m_keyDown {keyDown},
	m_polygon {pol},
	m_targetLine {pol->m_selectedLine}
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

void GridCreatingConditionTriangle::MovePolyLineCommand::redo()
{
	vtkPolyLine* pol = m_targetLine->getVtkLine();
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
	m_targetLine->updateShapeData();
	m_polygon->renderGraphicsView();
}

void GridCreatingConditionTriangle::MovePolyLineCommand::undo()
{
	vtkPolyLine* pol = m_targetLine->getVtkLine();
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
	m_targetLine->updateShapeData();
	m_polygon->renderGraphicsView();
}

int GridCreatingConditionTriangle::MovePolyLineCommand::id() const
{
	return iRIC::generateCommandId("TrianglePolyLineMove");
}

bool GridCreatingConditionTriangle::MovePolyLineCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolyLineCommand* comm = dynamic_cast<const MovePolyLineCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polygon != m_polygon) {return false;}
	if (comm->m_targetLine != m_targetLine) {return false;}
	m_offset += comm->m_offset;
	return true;
}
