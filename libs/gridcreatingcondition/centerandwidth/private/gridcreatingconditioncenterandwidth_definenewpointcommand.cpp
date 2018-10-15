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
	m_newPoint = QPointF(dx, dy);
}

void GridCreatingConditionCenterAndWidth::DefineNewPointCommand::redo()
{
	auto line = m_condition->polyLine();
	if (m_keyDown) {
		line.push_back(m_newPoint);
	} else {
		line[line.size() - 1] = m_newPoint;
	}
	m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meDefining;
	m_condition->setPolyLine(line);
	m_condition->updateShapeData();
}

void GridCreatingConditionCenterAndWidth::DefineNewPointCommand::undo()
{
	auto line = m_condition->polyLine();
	if (m_keyDown) {
		line.pop_back();
		if (line.size() == 0) {
			m_condition->m_mouseEventMode = GridCreatingConditionCenterAndWidth::meBeforeDefining;
		}
	} else {
		// this does not happen. no implementation needed.
	}
	m_condition->setPolyLine(line);
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
	return true;
}
