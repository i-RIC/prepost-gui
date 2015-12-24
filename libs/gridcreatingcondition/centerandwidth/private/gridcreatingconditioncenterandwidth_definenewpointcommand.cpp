#include "gridcreatingconditioncenterandwidth_definenewpointcommand.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionCenterAndWidth::DefineNewPointCommand::DefineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionCenterAndWidth* cond) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Add New Center Line Point")},
	m_keyDown {keyDown},
	m_condition {cond}
{
	double dx = point.x();
	double dy = point.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	m_newPoint = QVector2D(dx, dy);
}

void GridCreatingConditionCenterAndWidth::DefineNewPointCommand::redo()
{
	vtkPolyLine* line = m_condition->m_vtkPolyLine;
	if (m_keyDown) {
		// add new point.
		vtkIdType numOfPoints = line->GetPoints()->GetNumberOfPoints();
		if (numOfPoints == 0) {
			m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meDefining;
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
	m_condition->updateShapeData();
}

void GridCreatingConditionCenterAndWidth::DefineNewPointCommand::undo()
{
	vtkPolyLine* line = m_condition->m_vtkPolyLine;
	if (m_keyDown) {
		// decrease the number of points. i. e. remove the last point.
		vtkIdType numOfPoints = line->GetPoints()->GetNumberOfPoints();
		if (numOfPoints == 1) {
			m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meBeforeDefining;
		}
		line->GetPoints()->SetNumberOfPoints(numOfPoints - 1);
		line->GetPoints()->Modified();
	} else {
		// this does not happen. no implementation needed.
	}
	line->Modified();
	m_condition->updateShapeData();
}

int GridCreatingConditionCenterAndWidth::DefineNewPointCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthDefineNewPoint");
}

bool GridCreatingConditionCenterAndWidth::DefineNewPointCommand::mergeWith(const QUndoCommand* other)
{
	const DefineNewPointCommand* comm = dynamic_cast<const DefineNewPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_condition != m_condition) {return false;}

	m_newPoint = comm->m_newPoint;
}
