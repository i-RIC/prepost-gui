#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H

#include "../post3dwindowcontourgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowContourGroupDataItem::UpdateActorSettingsCommand : public QUndoCommand
{
public:
	UpdateActorSettingsCommand(bool apply, QUndoCommand* updateColorMapCommand, QUndoCommand* updateFacesCommand, Post3dWindowContourGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	QUndoCommand* m_updateColorMapCommand;
	QUndoCommand* m_updateFacesCommand;
	Post3dWindowContourGroupDataItem* m_item;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
