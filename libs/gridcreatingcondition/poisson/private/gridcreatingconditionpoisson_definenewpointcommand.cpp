#include "gridcreatingconditionpoisson_definenewpointcommand.h"
#include "gridcreatingconditionpoisson_impl.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionPoisson::DefineNewPointCommand::DefineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionPoisson* cond) :
	QUndoCommand {GridCreatingConditionPoisson::tr("Add New Line Point")},
	m_keyDown {keyDown},
	m_condition {cond}
{
	double x = point.x();
	double y = point.y();
	cond->graphicsView()->viewportToWorld(x, y);
	m_newPoint = QPointF(x, y);
}

void GridCreatingConditionPoisson::DefineNewPointCommand::redo()
{
	auto line = m_condition->impl->m_activePoints->polyLine();
	if (m_keyDown) {
		line.push_back(m_newPoint);
	} else {
		line[line.size() - 1] = m_newPoint;
	}
	m_condition->impl->m_mouseEventMode = GridCreatingConditionPoisson::Impl::MouseEventMode::Defining;
	m_condition->impl->m_activePoints->setPolyLine(line);
}

void GridCreatingConditionPoisson::DefineNewPointCommand::undo()
{
	auto line = m_condition->impl->m_activePoints->polyLine();
	if (m_keyDown) {
		line.pop_back();
		if (line.size() == 0) {
			m_condition->impl->m_mouseEventMode = GridCreatingConditionPoisson::Impl::MouseEventMode::BeforeDefining;
		}
	} else {
		// this does not happen. no implementation needed.
	}
	m_condition->impl->m_activePoints->setPolyLine(line);
}

int GridCreatingConditionPoisson::DefineNewPointCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionPoissonDefineNewPoint");
}

bool GridCreatingConditionPoisson::DefineNewPointCommand::mergeWith(const QUndoCommand* other)
{
	auto comm = dynamic_cast<const DefineNewPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_condition != m_condition) {return false;}

	m_newPoint = comm->m_newPoint;
	return true;
}
