#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_SETRANGESCOMMAND_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_SETRANGESCOMMAND_H

#include "../post3dwindowcontourgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowContourGroupDataItem::SetFacesCommand : public QUndoCommand
{
public:
	SetFacesCommand(const std::vector<Post3dWindowFaceSettingContainer>& ranges, Post3dWindowContourGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void apply(const std::vector<Post3dWindowFaceSettingContainer>& ranges);

	std::vector<Post3dWindowFaceSettingContainer> m_newFaces;
	std::vector<Post3dWindowFaceSettingContainer> m_oldFaces;

	Post3dWindowContourGroupDataItem* m_item;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_SETRANGESCOMMAND_H
