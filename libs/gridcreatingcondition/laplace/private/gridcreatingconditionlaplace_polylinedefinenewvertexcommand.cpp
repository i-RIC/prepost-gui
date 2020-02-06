#include "gridcreatingconditionlaplace_polylinedefinenewvertexcommand.h"

#include <guibase/polyline/polylinecontroller.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionLaplace::PolyLineDefineNewVertexCommand::PolyLineDefineNewVertexCommand(bool keyDown, const QPoint& point, PolyLineController* line, GridCreatingConditionLaplace* cond) :
	QUndoCommand {GridCreatingConditionLaplace::tr("Add New Point")},
	m_keyDown {keyDown},
	m_polyLine {line}
{
	double x = point.x();
	double y = point.y();
	cond->graphicsView()->viewportToWorld(x, y);
	m_point = QPointF(x, y);
}

void GridCreatingConditionLaplace::PolyLineDefineNewVertexCommand::redo()
{
	auto line = m_polyLine->polyLine();
	if (m_keyDown) {
		line.push_back(m_point);
	} else {
		line[line.size() - 1] = m_point;
	}
	m_polyLine->setPolyLine(line);
}

void GridCreatingConditionLaplace::PolyLineDefineNewVertexCommand::undo()
{
	auto line = m_polyLine->polyLine();
	if (m_keyDown) {
		line.pop_back();
	}
	m_polyLine->setPolyLine(line);
}

int GridCreatingConditionLaplace::PolyLineDefineNewVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionLaplacePolyLineDefineNewVertexCommand");
}

bool GridCreatingConditionLaplace::PolyLineDefineNewVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const PolyLineDefineNewVertexCommand*> (other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polyLine != m_polyLine) {return false;}

	m_point = comm->m_point;
	return true;
}
