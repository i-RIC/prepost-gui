#ifndef POST3DWINDOWCELLCONTOURGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H
#define POST3DWINDOWCELLCONTOURGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H

#include "../post3dwindowcellcontourgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowCellContourGroupDataItem::UpdateActorSettingCommand : public QUndoCommand
{
public:
	UpdateActorSettingCommand(bool apply, QUndoCommand* updateColorMapCommand, QUndoCommand* updateRangesCommand, Post3dWindowCellContourGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	QUndoCommand* m_updateColorMapCommand;
	QUndoCommand* m_updateRangesCommand;
	Post3dWindowCellContourGroupDataItem* m_item;
};

#endif // POST3DWINDOWCELLCONTOURGROUPDATAITEM_UPDATEACTORSETTINGCOMMAND_H
