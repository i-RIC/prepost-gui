#include "gridcreatingconditionlaplace_centerlinestartdefinitioncommand.h"
#include "gridcreatingconditionlaplace_impl.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionLaplace::CenterLineStartDefinitionCommand::CenterLineStartDefinitionCommand(bool keyDown, const QPoint& point, GridCreatingConditionLaplace* cond) :
	QUndoCommand(GridCreatingConditionLaplace::tr("Start defining center line")),
	m_keyDown {keyDown},
	m_condition {cond}
{
	double x = point.x();
	double y = point.y();
	cond->graphicsView()->viewportToWorld(x, y);
	m_position = QPointF(x, y);
}

void GridCreatingConditionLaplace::CenterLineStartDefinitionCommand::redo()
{
	if (m_keyDown) {
		std::vector<QPointF> line;
		line.push_back(m_position);
		line.push_back(m_position);
		m_condition->impl->m_centerLine.setPolyLine(line);
		m_condition->impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::Defining;
	} else {
		auto line = m_condition->impl->m_centerLine.polyLine();
		line[line.size() - 1] = m_position;
		m_condition->impl->m_centerLine.setPolyLine(line);
	}
}

void GridCreatingConditionLaplace::CenterLineStartDefinitionCommand::undo()
{
	m_condition->impl->m_centerLine.polyLine().clear();
	m_condition->impl->m_centerLineOnlyMouseEventMode = Impl::CenterLineOnlyMouseEventMode::BeforeDefining;
}

int GridCreatingConditionLaplace::CenterLineStartDefinitionCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionLaplaceCenterLineStartDefinition");
}

bool GridCreatingConditionLaplace::CenterLineStartDefinitionCommand::mergeWith(const QUndoCommand* other)
{
	const auto* comm = dynamic_cast<const CenterLineStartDefinitionCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_condition != m_condition) {return false;}

	return true;
}
