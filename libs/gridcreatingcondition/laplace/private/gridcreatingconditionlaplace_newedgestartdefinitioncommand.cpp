#include "gridcreatingconditionlaplace_newedgestartdefinitioncommand.h"
#include "gridcreatingconditionlaplace_impl.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionLaplace::NewEdgeStartDefinitionCommand::NewEdgeStartDefinitionCommand(const QPoint& point, GridCreatingConditionLaplace* cond) :
	QUndoCommand(GridCreatingConditionLaplace::tr("Start adding new edge")),
	m_keyDown {false},
	m_condition {cond}
{
	double x = point.x();
	double y = point.y();
	cond->graphicsView()->viewportToWorld(x, y);
	m_position = QPointF(x, y);
}

GridCreatingConditionLaplace::NewEdgeStartDefinitionCommand::NewEdgeStartDefinitionCommand(const QPointF& pos, GridCreatingConditionLaplace* cond) :
	QUndoCommand(GridCreatingConditionLaplace::tr("Start adding new edge")),
	m_keyDown {true},
	m_position {pos},
	m_condition {cond}
{}

void GridCreatingConditionLaplace::NewEdgeStartDefinitionCommand::redo()
{
	m_condition->impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::AddEdgeLine;
	if (m_keyDown) {
		std::vector<QPointF> line;
		line.push_back(m_position);
		line.push_back(m_position);
		m_condition->impl->m_newEdgeLine.setPolyLine(line);
	} else {
		auto line = m_condition->impl->m_newEdgeLine.polyLine();
		line[line.size() - 1] = m_position;
		m_condition->impl->m_newEdgeLine.setPolyLine(line);
	}
	m_condition->impl->updateActorSetting();
}

void GridCreatingConditionLaplace::NewEdgeStartDefinitionCommand::undo()
{
	m_condition->impl->m_regionDefinedMouseEventMode = Impl::RegionDefinedMouseEventMode::Normal;
	std::vector<QPointF> empty;
	m_condition->impl->m_newEdgeLine.setPolyLine(empty);
	m_condition->impl->updateActorSetting();
}

int GridCreatingConditionLaplace::NewEdgeStartDefinitionCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionLaplaceNewEdgeStartDefinition");
}

bool GridCreatingConditionLaplace::NewEdgeStartDefinitionCommand::mergeWith(const QUndoCommand* other)
{
	const auto* comm = dynamic_cast<const NewEdgeStartDefinitionCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_condition != m_condition) { return false; }

	return true;
}
