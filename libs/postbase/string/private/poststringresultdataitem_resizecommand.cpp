#include "poststringresultdataitem_resizecommand.h"

#include <misc/qundocommandhelper.h>

PostStringResultDataItem::ResizeCommand::ResizeCommand(bool keyDown, const QPoint& from, const QPoint& to, PostStringResultDataItem* item) :
	QUndoCommand(PostStringResultDataItem::tr("Resize label")),
	m_keyDown {keyDown},
	m_dw {to.x() - from.x()},
	m_dh {to.y() - from.y()},
	m_oldAuto {item->m_setting.autoSize},
	m_item {item}
{}

void PostStringResultDataItem::ResizeCommand::redo()
{
	m_item->m_setting.width = m_item->m_setting.width.value() + m_dw;
	m_item->m_setting.height = m_item->m_setting.height.value() + m_dh;
	m_item->m_setting.autoSize = false;
	m_item->update();
}

void PostStringResultDataItem::ResizeCommand::undo()
{
	m_item->m_setting.width = m_item->m_setting.width.value() - m_dw;
	m_item->m_setting.height = m_item->m_setting.height.value() - m_dh;
	m_item->m_setting.autoSize = m_oldAuto;
	m_item->update();
}

int PostStringResultDataItem::ResizeCommand::id() const
{
	return iRIC::generateCommandId("PostStringResultDataItemResize");
}

bool PostStringResultDataItem::ResizeCommand::mergeWith(const QUndoCommand *other)
{
	const auto comm = dynamic_cast<const ResizeCommand*> (other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_item != m_item) {return false;}
	m_dw += comm->m_dw;
	m_dh += comm->m_dh;
	return true;
}
