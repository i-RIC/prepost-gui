#include "poststringresultdataitem_movecommand.h"

#include <misc/qundocommandhelper.h>

PostStringResultDataItem::MoveCommand::MoveCommand(bool keyDown, const QPoint& from, const QPoint& to, PostStringResultDataItem* item) :
	QUndoCommand(PostStringResultDataItem::tr("Move label")),
	m_keyDown {keyDown},
	m_dx {to.x() - from.x()},
	m_dy {to.y() - from.y()},
	m_item {item}
{}

void PostStringResultDataItem::MoveCommand::redo()
{
	m_item->m_setting.left = m_item->m_setting.left.value() + m_dx;
	m_item->m_setting.top = m_item->m_setting.top.value() + m_dy;
	m_item->update();
}

void PostStringResultDataItem::MoveCommand::undo()
{
	m_item->m_setting.left = m_item->m_setting.left.value() - m_dx;
	m_item->m_setting.top = m_item->m_setting.top.value() - m_dy;
	m_item->update();
}

int PostStringResultDataItem::MoveCommand::id() const
{
	return iRIC::generateCommandId("PostStringResultDataItemMove");
}

bool PostStringResultDataItem::MoveCommand::mergeWith(const QUndoCommand *other)
{
	const auto comm = dynamic_cast<const MoveCommand*> (other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_item != m_item) {return false;}
	m_dx += comm->m_dx;
	m_dy += comm->m_dy;
	return true;
}
