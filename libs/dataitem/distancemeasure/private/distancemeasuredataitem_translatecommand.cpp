#include "distancemeasuredataitem_translatecommand.h"
#include "distancemeasuredataitem_impl.h"

#include <misc/qundocommandhelper.h>

DistanceMeasureDataItem::TranslateCommand::TranslateCommand(const QPointF& p1, const QPointF& p2, bool finish, DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Translate Distance Measure")),
	m_newPoint1 {p1},
	m_newPoint2 {p2},
	m_oldPoint1{ item->impl->m_setting.point1 },
	m_oldPoint2{ item->impl->m_setting.point2 },
	m_finish {finish},
	m_item {item}
{}

void DistanceMeasureDataItem::TranslateCommand::redo()
{
	applyPoints(m_newPoint1, m_newPoint2);

	if (m_finish) {
		m_item->impl->m_mouseEventMode = Impl::meNormal;
	} else {
		m_item->impl->m_mouseEventMode = Impl::meTranslate;
	}
	m_item->updateMouseCursor();
}

void DistanceMeasureDataItem::TranslateCommand::undo()
{
	applyPoints(m_oldPoint1, m_oldPoint2);

	m_item->impl->m_mouseEventMode = Impl::meNormal;
	m_item->updateMouseCursor();
}

int DistanceMeasureDataItem::TranslateCommand::id() const
{
	return iRIC::generateCommandId("DistanceMeasureTranslateCommand");
}

bool DistanceMeasureDataItem::TranslateCommand::mergeWith(const QUndoCommand *other)
{
	const TranslateCommand* other2 = dynamic_cast<const TranslateCommand*>(other);
	if (other2 == nullptr) {return false;}

	if (m_finish) {return false;}

	m_newPoint1 = other2->m_newPoint1;
	m_newPoint2 = other2->m_newPoint2;
	m_finish = other2->m_finish;
	return true;
}

void DistanceMeasureDataItem::TranslateCommand::applyPoints(const QPointF& p1, const QPointF& p2)
{
	m_item->impl->m_setting.point1 = p1;
	m_item->impl->m_setting.point2 = p2;

	m_item->updateActorSettings();
}
