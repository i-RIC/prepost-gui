#include "gridcreatingconditioncompoundchannel_addpolylinevertexcommand.h"
#include "../gridcreatingconditioncompoundchannelabstractline.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionCompoundChannel::AddPolyLineVertexCommand::AddPolyLineVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Insert Polygonal Line Vertex")},
	m_keyDown {keyDown},
	m_vertexId {edgeId + 1},
	m_targetLine {cond->m_selectedLine},
	m_condition {cond}
{
	double dx = point.x();
	double dy = point.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	m_vertexPosition = QPointF(dx, dy);
}

void GridCreatingConditionCompoundChannel::AddPolyLineVertexCommand::redo()
{
	if (m_keyDown) {
		// add vertex.
		m_targetLine->insertVertex(m_vertexId, m_vertexPosition);
	} else {
		// just modify the vertex position
		m_targetLine->setVertex(m_vertexId, m_vertexPosition);
	}
}

void GridCreatingConditionCompoundChannel::AddPolyLineVertexCommand::undo()
{
	if (m_keyDown) {
		// remove vertex.
		m_targetLine->removeVertex(m_vertexId);
	} else {
		// this never happens.
	}
}

int GridCreatingConditionCompoundChannel::AddPolyLineVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCompoundChannelAddPolyLineVertex");
}

bool GridCreatingConditionCompoundChannel::AddPolyLineVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddPolyLineVertexCommand* comm = dynamic_cast<const AddPolyLineVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_targetLine != comm->m_targetLine) {return false;}
	if (m_condition != comm->m_condition) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}
	m_vertexPosition = comm->m_vertexPosition;
	return true;
}
