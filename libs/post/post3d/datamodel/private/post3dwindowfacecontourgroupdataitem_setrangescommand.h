#ifndef POST3DWINDOWFACECONTOURGROUPDATAITEM_SETRANGESCOMMAND_H
#define POST3DWINDOWFACECONTOURGROUPDATAITEM_SETRANGESCOMMAND_H

#include "../post3dwindowFacecontourgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowFaceContourGroupDataItem::SetRangesCommand : public QUndoCommand
{
public:
	SetRangesCommand(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges, Post3dWindowFaceContourGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void apply(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges);

	std::vector<Post3dWindowCellRangeSettingContainer> m_newRanges;
	std::vector<Post3dWindowCellRangeSettingContainer> m_oldRanges;

	Post3dWindowFaceContourGroupDataItem* m_item;
};

#endif // POST3DWINDOWFACECONTOURGROUPDATAITEM_SETRANGESCOMMAND_H
