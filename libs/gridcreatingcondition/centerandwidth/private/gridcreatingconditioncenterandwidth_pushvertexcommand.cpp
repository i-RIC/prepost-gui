#include "gridcreatingconditioncenterandwidth_pushvertexcommand.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionCenterAndWidth::PushVertexCommand::PushVertexCommand(bool keyDown, const QPoint& point, GridCreatingConditionCenterAndWidth* cond) :
	PolyLinePushVertexCommand(GridCreatingConditionCenterAndWidth::tr("Add New Center Line Point"), keyDown, cond->graphicsView()->viewportToWorld(point), &(cond->m_polyLineController)),
	m_condition {cond}
{}

void GridCreatingConditionCenterAndWidth::PushVertexCommand::redo()
{
	PolyLinePushVertexCommand::redo();
	m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meDefining;
}

void GridCreatingConditionCenterAndWidth::PushVertexCommand::undo()
{
	PolyLinePushVertexCommand::undo();
	if (m_condition->polyLine().size() == 0) {
		m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meBeforeDefining;
	}
}
