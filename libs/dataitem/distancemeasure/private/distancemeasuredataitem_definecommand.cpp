#include "distancemeasuredataitem_definecommand.h"
#include "distancemeasuredataitem_impl.h"

#include <misc/qundocommandhelper.h>

DistanceMeasureDataItem::DefineCommand::DefineCommand(const QPointF& v1, const QPointF& v2, bool finish, DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Define Distance Measure")),
	m_newPoint1 {v1},
	m_newPoint2 {v2},
	m_oldPoint1 {item->impl->m_setting.point1},
	m_oldPoint2 {item->impl->m_setting.point2},
	m_finish {finish},
	m_item {item}
{}

void DistanceMeasureDataItem::DefineCommand::redo()
{
	DistanceMeasureDataItem::Impl::MouseEventMode mode = DistanceMeasureDataItem::Impl::meDefining;
	if (m_finish) {
		mode = DistanceMeasureDataItem::Impl::meNormal;
	}
	apply(m_newPoint1, m_newPoint2, mode, true);
}

void DistanceMeasureDataItem::DefineCommand::undo()
{
	apply(m_oldPoint1, m_oldPoint2, DistanceMeasureDataItem::Impl::meBeforeDefining, false);
}

int DistanceMeasureDataItem::DefineCommand::id() const
{
	return iRIC::generateCommandId("DistanceMeasureDefineCommand");
}

bool DistanceMeasureDataItem::DefineCommand::mergeWith(const QUndoCommand* other)
{
	const DefineCommand* other2 = dynamic_cast<const DefineCommand*>(other);
	if (other2 == nullptr) {return false;}

	m_newPoint1 = other2->m_newPoint1;
	m_newPoint2 = other2->m_newPoint2;
	m_finish = other2->m_finish;
	return true;
}

void DistanceMeasureDataItem::DefineCommand::apply(const QPointF& p1, const QPointF& p2, Impl::MouseEventMode mode, bool defined)
{
	m_item->impl->m_setting.point1 = p1;
	m_item->impl->m_setting.point2 = p2;
	m_item->impl->m_mouseEventMode = mode;
	m_item->impl->m_setting.defined = defined;

	m_item->updateMouseCursor();
	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}
