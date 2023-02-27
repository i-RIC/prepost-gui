#ifndef POST3DWINDOWCELLCONTOURGROUPDATAITEM_SETRANGESCOMMAND_H
#define POST3DWINDOWCELLCONTOURGROUPDATAITEM_SETRANGESCOMMAND_H

#include "../post3dwindowcellcontourgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowCellContourGroupDataItem::SetRangesCommand : public QUndoCommand
{
public:
	SetRangesCommand(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges, Post3dWindowCellContourGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void apply(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges);

	std::vector<Post3dWindowCellRangeSettingContainer> m_newRanges;
	std::vector<Post3dWindowCellRangeSettingContainer> m_oldRanges;

	Post3dWindowCellContourGroupDataItem* m_item;
};

#endif // POST3DWINDOWCELLCONTOURGROUPDATAITEM_SETRANGESCOMMAND_H
