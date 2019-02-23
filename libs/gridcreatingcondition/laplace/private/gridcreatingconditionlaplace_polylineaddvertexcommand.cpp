#include "gridcreatingconditionlaplace_polylineaddvertexcommand.h"

#include <guibase/polyline/polylinecontroller.h>
#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionLaplace::PolyLineAddVertexCommand::PolyLineAddVertexCommand(bool keyDown, PolyLineController* line, int edgeId, QPoint point, GridCreatingConditionLaplace* cond) :
	QUndoCommand(GridCreatingConditionLaplace::tr("Insert Line Vertex")),
	m_keyDown {keyDown},
	m_vertexId {edgeId + 1},
	m_polyline {line}
{
	double x = point.x();
	double y = point.y();
	cond->graphicsView()->viewportToWorld(x, y);
	m_position = QPointF(x, y);
}

void GridCreatingConditionLaplace::PolyLineAddVertexCommand::redo()
{
	auto line = m_polyline->polyLine();
	if (m_keyDown) {
		// add vertex
		auto it = line.begin() + m_vertexId;
		line.insert(it, m_position);
	} else {
		// just modify the vertex position
		line[m_vertexId] = m_position;
	}
	m_polyline->setPolyLine(line);
}

void GridCreatingConditionLaplace::PolyLineAddVertexCommand::undo()
{
	if (m_keyDown) {
		auto line = m_polyline->polyLine();
		auto it = line.begin() + m_vertexId;
		line.erase(it);
		m_polyline->setPolyLine(line);
	} else {
		// this never happens.
	}
}

int GridCreatingConditionLaplace::PolyLineAddVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionLaplacePolyLineAddVertex");
}

bool GridCreatingConditionLaplace::PolyLineAddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const PolyLineAddVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_polyline != comm->m_polyline) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}

	m_position = comm->m_position;
	return true;
}
