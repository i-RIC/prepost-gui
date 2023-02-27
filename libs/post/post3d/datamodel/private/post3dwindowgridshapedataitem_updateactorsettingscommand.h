#ifndef POST3DWINDOWGRIDSHAPEDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
#define POST3DWINDOWGRIDSHAPEDATAITEM_UPDATEACTORSETTINGSCOMMAND_H

#include "../post3dwindowgridshapedataitem.h"

#include <QUndoCommand>
#include <memory>

class Post3dWindowGridShapeDataItem::UpdateActorSettingsCommand : public QUndoCommand
{
public:
	UpdateActorSettingsCommand(bool apply, QUndoCommand* command, Post3dWindowGridShapeDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;
	std::unique_ptr<QUndoCommand> m_command;
	Post3dWindowGridShapeDataItem* m_item;
};

#endif // POST3DWINDOWGRIDSHAPEDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
