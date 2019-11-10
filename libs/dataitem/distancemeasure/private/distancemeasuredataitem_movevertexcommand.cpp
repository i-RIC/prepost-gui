#include "distancemeasuredataitem_movevertexcommand.h"

#include <misc/qundocommandhelper.h>

DistanceMeasureDataItem::MoveVertexCommand::MoveVertexCommand(int point, const QPointF& v, bool finish, DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Move Distance Measure Point")),
	m_point {point},
	m_newPoint {v},
	m_finish {finish},
	m_item {item}
{
	if (point == 1) {
		m_oldPoint = m_item->impl->m_setting.point1;
	} else if (point == 2) {
		m_oldPoint = m_item->impl->m_setting.point2;
	}
}

void DistanceMeasureDataItem::MoveVertexCommand::redo()
{
	Impl::MouseEventMode mode = Impl::meMoveVertex;
	if (m_finish) {
		mode = Impl::meNormal;
	}
	apply(m_newPoint, mode);
}

void DistanceMeasureDataItem::MoveVertexCommand::undo()
{
	apply(m_oldPoint, Impl::meNormal);
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

void DistanceMeasureDataItem::MoveVertexCommand::apply(const QPointF& p, Impl::MouseEventMode mode)
{
	if (m_point == 1) {
		m_item->impl->m_setting.point1 = p;
	} else if (m_point == 2) {
		m_item->impl->m_setting.point2 = p;
	}
	m_item->impl->m_mouseEventMode = mode;

	m_item->updateMouseCursor();
	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}
