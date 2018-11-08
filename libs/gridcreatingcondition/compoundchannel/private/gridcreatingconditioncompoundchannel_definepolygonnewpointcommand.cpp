#include "gridcreatingconditioncompoundchannel_definepolygonnewpointcommand.h"
#include "../gridcreatingconditioncompoundchannelabstractpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionCompoundChannel::DefinePolygonNewPointCommand::DefinePolygonNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Add New Polygon Point")},
	m_keyDown {keyDown},
	m_targetPolygon {cond->m_selectedPolygon},
	m_condition {cond}
{
	double dx = point.x();
	double dy = point.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	m_newPoint = QPointF(dx, dy);
}

void GridCreatingConditionCompoundChannel::DefinePolygonNewPointCommand::redo()
{
	if (m_keyDown) {
		// add new point.
		vtkIdType num = m_targetPolygon->getVtkPolygon()->GetNumberOfPoints();
		m_targetPolygon->insertVertex(num, m_newPoint);
	} else {
		// modify the last point.
		vtkIdType num = m_targetPolygon->getVtkPolygon()->GetNumberOfPoints();
		m_targetPolygon->setVertex(num - 1, m_newPoint);
	}
}

void GridCreatingConditionCompoundChannel::DefinePolygonNewPointCommand::undo()
{
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	if (m_keyDown) {
		// remove the last point
		vtkIdType num = m_targetPolygon->getVtkPolygon()->GetNumberOfPoints();
		m_targetPolygon->removeVertex(num - 1);
	} else {
		// this does not happen. no implementation needed.
	}
}

int GridCreatingConditionCompoundChannel::DefinePolygonNewPointCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCompoundChannelPolygonDefineNewPoint");
}

bool GridCreatingConditionCompoundChannel::DefinePolygonNewPointCommand::mergeWith(const QUndoCommand* other)
{
	const DefinePolygonNewPointCommand* comm = dynamic_cast<const DefinePolygonNewPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_targetPolygon != m_targetPolygon) {return false;}
	if (comm->m_condition != m_condition) {return false;}
	m_newPoint = comm->m_newPoint;
	return true;
}
