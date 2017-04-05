#include "gridcreatingconditioncenterandwidth_addvertexcommand.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

#include <vtkPolyData.h>

GridCreatingConditionCenterAndWidth::AddVertexCommand::AddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCenterAndWidth* cond) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Insert Center Line Vertex")},
	m_keyDown {keyDown},
	m_vertexId {edgeId + 1},
	m_condition {cond}
{
	double dx = point.x();
	double dy = point.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	m_vertexPosition = QPointF(dx, dy);
}

void GridCreatingConditionCenterAndWidth::AddVertexCommand::redo()
{
	auto line = m_condition->polyLine();
	if (m_keyDown) {
		// add vertex
		auto it = line.begin() + m_vertexId;
		line.insert(it, m_vertexPosition);
	} else {
		// just modify the vertex position
		line[m_vertexId] = m_vertexPosition;
	}
	m_condition->setPolyLine(line);
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::AddVertexCommand::undo()
{
	if (m_keyDown) {
		auto line = m_condition->polyLine();
		auto it = line.begin() + m_vertexId;
		line.erase(it);
		m_condition->setPolyLine(line);
		if (m_condition->m_isGridCreated) {
			m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
			emit m_condition->tmpGridCreated(m_condition->createGrid());
		}
	} else {
		// this never happens.
	}
}

int GridCreatingConditionCenterAndWidth::AddVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthAddVertex");
}

bool GridCreatingConditionCenterAndWidth::AddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddVertexCommand* comm = dynamic_cast<const AddVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_condition != comm->m_condition) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}

	m_vertexPosition = comm->m_vertexPosition;
	return true;
}
