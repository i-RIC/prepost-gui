#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H

#include "../post2dwindowpolydatagroupdataitem.h"

#include <QUndoCommand>

class Post2dWindowPolyDataGroupDataItem::UpdateActorSettingsCommand : public QUndoCommand
{
public:
	UpdateActorSettingsCommand(bool apply, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand, Post2dWindowPolyDataGroupDataItem* item);
	~UpdateActorSettingsCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;

	QUndoCommand* m_settingCommand;
	QUndoCommand* m_colorMapCommand;

	Post2dWindowPolyDataGroupDataItem* m_item;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
