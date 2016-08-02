#include "distancemeasuredataitem_movevertexcommand.h"

#include <guicore/misc/qundocommandhelper.h>

DistanceMeasureDataItem::MoveVertexCommand::MoveVertexCommand(int point, const QVector2D& v, bool finish, DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Move Distance Measure Point")),
	m_point {point},
	m_newPoint {v},
	m_finish {finish},
	m_item {item}
{
	if (point == 1) {
		m_oldPoint = m_item->m_point1;
	} else if (point == 2) {
		m_oldPoint = m_item->m_point2;
	}
}

void DistanceMeasureDataItem::MoveVertexCommand::redo()
{
	if (m_point == 1) {
		m_item->m_point1 = m_newPoint;
	} else if (m_point == 2) {
		m_item->m_point2 = m_newPoint;
	}
	if (m_finish) {
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;
	} else {
		m_item->m_mouseEventMode = DistanceMeasureDataItem::meMoveVertex;
	}
	m_item->updateMouseCursor();
	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}

void DistanceMeasureDataItem::MoveVertexCommand::undo()
{
	if (m_point == 1) {
		m_item->m_point1 = m_oldPoint;
	} else if (m_point == 2) {
		m_item->m_point2 = m_oldPoint;
	}
	m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;

	m_item->updateMouseCursor();
	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}

int DistanceMeasureDataItem::MoveVertexCommand::id() const
{
	return iRIC::generateCommandId("DistanceMeasureMoveVertexCommand");
}

bool DistanceMeasureDataItem::MoveVertexCommand::mergeWith(const QUndoCommand* other)
{
	const MoveVertexCommand* other2 = dynamic_cast<const MoveVertexCommand*>(other);
	if (other2 == nullptr) {return false;}
	if (m_point != other2->m_point) {return false;}
	if (m_finish) {return false;}

	m_newPoint = other2->m_newPoint;
	m_finish = other2->m_finish;
	return true;
}
