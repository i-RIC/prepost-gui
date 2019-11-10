#include "gridcreatingconditioncenterandwidth_addvertexcommand.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

#include <vtkPolyData.h>

GridCreatingConditionCenterAndWidth::AddVertexCommand::AddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCenterAndWidth* cond) :
	PolyLineAddVertexCommand {GridCreatingConditionCenterAndWidth::tr("Insert Center Line Vertex"), keyDown, edgeId, cond->graphicsView()->viewportToWorld(point), &(cond->m_polyLineController)}
{}

void GridCreatingConditionCenterAndWidth::AddVertexCommand::redo()
{
	PolyLineAddVertexCommand::redo();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::AddVertexCommand::undo()
{
	PolyLineAddVertexCommand::undo();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

int GridCreatingConditionCenterAndWidth::AddVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthAddVertex");
}

bool GridCreatingConditionCenterAndWidth::AddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddVertexCommand* comm = dynamic_cast<const AddVertexCommand*>(other);
	if (m_condition != comm->m_condition) {return false;}
	return PolyLineAddVertexCommand::mergeWith(other);
}
