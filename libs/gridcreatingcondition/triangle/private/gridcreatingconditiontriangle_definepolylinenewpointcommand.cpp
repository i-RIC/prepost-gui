#include "gridcreatingconditiontriangle_definepolylinenewpointcommand.h"
#include "../gridcreatingconditiontriangleabstractline.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionTriangle::DefinePolyLineNewPointCommand::DefinePolyLineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionTriangle* cond) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Add New Break Line Point")},
	m_keyDown {keyDown},
	m_condition {cond},
	m_targetLine {m_condition->m_selectedLine}
{
	double dx = point.x();
	double dy = point.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	m_newPoint = QVector2D(dx, dy);
}

void GridCreatingConditionTriangle::DefinePolyLineNewPointCommand::redo()
{
	vtkPolyLine* line = m_targetLine->getVtkLine();
	if (m_keyDown) {
		// add new point.
		vtkIdType numOfPoints = line->GetPoints()->GetNumberOfPoints();
		if (numOfPoints == 0) {
			m_condition->m_mouseEventMode = GridCreatingConditionTriangle::meDefining;
		}
		line->GetPoints()->InsertNextPoint(m_newPoint.x(), m_newPoint.y(), 0);
		line->GetPoints()->Modified();
	} else {
		// modify the last point.
		vtkIdType lastId = line->GetNumberOfPoints() - 1;
		line->GetPoints()->SetPoint(lastId, m_newPoint.x(), m_newPoint.y(), 0);
		line->GetPoints()->Modified();
	}
	line->Modified();
	m_targetLine->updateShapeData();
	m_condition->renderGraphicsView();
}

void GridCreatingConditionTriangle::DefinePolyLineNewPointCommand::undo()
{
	vtkPolyLine* line = m_targetLine->getVtkLine();
	if (m_keyDown) {
		// decrease the number of points. i. e. remove the last point.
		vtkIdType numOfPoints = line->GetPoints()->GetNumberOfPoints();
		if (numOfPoints == 1) {
			m_condition->m_mouseEventMode = GridCreatingConditionTriangle::meBeforeDefining;
		}
		line->GetPoints()->SetNumberOfPoints(numOfPoints - 1);
		line->GetPoints()->Modified();
	} else {
		// this does not happen. no implementation needed.
	}
	line->Modified();
	m_targetLine->updateShapeData();
	m_condition->renderGraphicsView();
}

int GridCreatingConditionTriangle::DefinePolyLineNewPointCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionTrianglePolyLineDefineNewPoint");
}

bool GridCreatingConditionTriangle::DefinePolyLineNewPointCommand::mergeWith(const QUndoCommand* other)
{
	const DefinePolyLineNewPointCommand* comm = dynamic_cast<const DefinePolyLineNewPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_condition != m_condition) {return false;}
	m_newPoint = comm->m_newPoint;
	return true;
}
