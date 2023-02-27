#ifndef POST3DWINDOWCELLCONTOURGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
#define POST3DWINDOWCELLCONTOURGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H

#include "../post3dwindowcellcontourgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowCellContourGroupDataItem::UpdateActorSettingsCommand : public QUndoCommand
{
public:
	UpdateActorSettingsCommand(bool apply, QUndoCommand* updateColorMapCommand, QUndoCommand* updateRangesCommand, Post3dWindowCellContourGroupDataItem* item);

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

#endif // POST3DWINDOWCELLCONTOURGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
