#include "gridcreatingconditioncompoundchannel_definepolylinenewpointcommand.h"
#include "../gridcreatingconditioncompoundchannelabstractline.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionCompoundChannel::DefinePolyLineNewPointCommand::DefinePolyLineNewPointCommand(bool keyDown, const QPoint& point, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Add New Center Line Point")},
	m_keyDown {keyDown},
	m_targetLine {cond->m_selectedLine},
	m_condition {cond}
{
	double dx = point.x();
	double dy = point.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	m_newPoint = QPointF(dx, dy);
}

void GridCreatingConditionCompoundChannel::DefinePolyLineNewPointCommand::redo()
{
	if (m_keyDown) {
		// add new point.
		vtkIdType num = m_targetLine->getVtkLine()->GetNumberOfPoints();
		m_targetLine->insertVertex(num, m_newPoint);
		if (num == 0) {
			m_condition->m_mouseEventMode = GridCreatingConditionCompoundChannel::meDefining;
		}
	} else {
		// modify the last point.
		vtkIdType num = m_targetLine->getVtkLine()->GetNumberOfPoints();
		m_targetLine->setVertex(num - 1, m_newPoint);
	}
}

void GridCreatingConditionCompoundChannel::DefinePolyLineNewPointCommand::undo() {
	vtkPolyLine* line = m_targetLine->getVtkLine();
	if (m_keyDown) {
		// remove the last point.
		vtkIdType num = m_targetLine->getVtkLine()->GetNumberOfPoints();
		m_targetLine->removeVertex(num - 1);
		if (num == 1) {
			m_condition->m_mouseEventMode = GridCreatingConditionCompoundChannel::meBeforeDefining;
		}
	} else {
		// this does not happen. no implementation needed.
	}
}

int GridCreatingConditionCompoundChannel::DefinePolyLineNewPointCommand::id() const {
	return iRIC::generateCommandId("GridCreatingConditionCompoundChannelDefinePolyLineNewPoint");
}

bool GridCreatingConditionCompoundChannel::DefinePolyLineNewPointCommand::mergeWith(const QUndoCommand* other) {
	const DefinePolyLineNewPointCommand* comm = dynamic_cast<const DefinePolyLineNewPointCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_targetLine != m_targetLine) {return false;}
	if (comm->m_condition != m_condition) {return false;}
	m_newPoint = comm->m_newPoint;
	return true;
}
