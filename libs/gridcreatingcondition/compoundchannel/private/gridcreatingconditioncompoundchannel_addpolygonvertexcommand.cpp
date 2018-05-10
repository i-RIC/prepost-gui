#include "gridcreatingconditioncompoundchannel_addpolygonvertexcommand.h"
#include "../gridcreatingconditioncompoundchannelabstractpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionCompoundChannel::AddPolygonVertexCommand::AddPolygonVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Insert Polygon Vertex")},
	m_keyDown {keyDown},
	m_targetPolygon {cond->m_selectedPolygon},
	m_condition {cond}
{
	m_vertexId = (edgeId + 1) % (cond->m_selectedPolygon->getVtkPolygon()->GetNumberOfPoints() + 1);
	double dx = point.x();
	double dy = point.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	m_vertexPosition = QPointF(dx, dy);
}

void GridCreatingConditionCompoundChannel::AddPolygonVertexCommand::redo()
{
	if (m_keyDown) {
		// add vertex
		m_targetPolygon->insertVertex(m_vertexId, m_vertexPosition);
	} else {
		// just modify the vertex position
		m_targetPolygon->setVertex(m_vertexId, m_vertexPosition);
	}
}

void GridCreatingConditionCompoundChannel::AddPolygonVertexCommand::undo()
{
	if (m_keyDown) {
		// remove vertex at m_vertexId
		m_targetPolygon->removeVertex(m_vertexId);
	} else {
		// this never happens.
	}
}

int GridCreatingConditionCompoundChannel::AddPolygonVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCompoundChannelAddPolygonVertex");
}

bool GridCreatingConditionCompoundChannel::AddPolygonVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddPolygonVertexCommand* comm = dynamic_cast<const AddPolygonVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}
	if (m_targetPolygon != comm->m_targetPolygon) {return false;}
	if (m_condition != comm->m_condition) {return false;}
	m_vertexPosition = comm->m_vertexPosition;
	return true;
}
