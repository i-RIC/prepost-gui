#include "post3dwindowcellcontourgroupdataitem_setrangescommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowCellContourGroupDataItem::SetRangesCommand::SetRangesCommand(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges, Post3dWindowCellContourGroupDataItem* item) :
	QUndoCommand("Update ranges"),
	m_newRanges {ranges},
	m_oldRanges {item->ranges()},
	m_item {item}
{}

void Post3dWindowCellContourGroupDataItem::SetRangesCommand::redo()
{
	apply(m_newRanges);
}

void Post3dWindowCellContourGroupDataItem::SetRangesCommand::undo()
{
	apply(m_oldRanges);
}

int Post3dWindowCellContourGroupDataItem::SetRangesCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowCellContourGroupDataItem::SetRangesCommand");
}

bool Post3dWindowCellContourGroupDataItem::SetRangesCommand::mergeWith(const QUndoCommand* other)
{
	auto other2 = dynamic_cast<const SetRangesCommand*> (other);
	if (other2 == nullptr) {return false;}

	if (m_item != other2->m_item) {return false;}
	m_newRanges = other2->m_newRanges;

	return true;
}

void Post3dWindowCellContourGroupDataItem::SetRangesCommand::apply(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges)
{
	m_item->setRanges(ranges);
}
