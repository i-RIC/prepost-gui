#include "gridcreatingconditionpoisson_addvertexcommand.h"
#include "gridcreatingconditionpoisson_impl.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

#include <vtkPolyData.h>

GridCreatingConditionPoisson::AddVertexCommand::AddVertexCommand(bool keyDown, int edgeId, QPoint point, GridCreatingConditionPoisson* cond) :
	QUndoCommand {GridCreatingConditionPoisson::tr("Insert Line Vertex")},
	m_keyDown {keyDown},
	m_vertexId {edgeId + 1},
	m_polyLine {cond->impl->m_activePoints}
{
	double x = point.x();
	double y = point.y();
	cond->graphicsView()->viewportToWorld(x, y);
	m_vertexPosition = QPointF(x, y);
}

void GridCreatingConditionPoisson::AddVertexCommand::redo()
{
	auto line = m_polyLine->polyLine();
	if (m_keyDown) {
		// add vertex
		auto it = line.begin() + m_vertexId;
		line.insert(it, m_vertexPosition);
	} else {
		// just modify the vertex position
		line[m_vertexId] = m_vertexPosition;
	}
	m_polyLine->setPolyLine(line);
}

void GridCreatingConditionPoisson::AddVertexCommand::undo()
{
	if (m_keyDown) {
		auto line = m_polyLine->polyLine();
		auto it = line.begin() + m_vertexId;
		line.erase(it);
		m_polyLine->setPolyLine(line);
	} else {
		// this never happens.
	}
}

int GridCreatingConditionPoisson::AddVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionPoissonAddVertex");
}

bool GridCreatingConditionPoisson::AddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const AddVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_polyLine != comm->m_polyLine) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}

	m_vertexPosition = comm->m_vertexPosition;
	return true;
}
