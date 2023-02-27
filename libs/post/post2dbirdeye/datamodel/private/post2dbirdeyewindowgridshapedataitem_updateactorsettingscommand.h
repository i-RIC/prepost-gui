#ifndef POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
#define POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_UPDATEACTORSETTINGSCOMMAND_H

#include "../post2dbirdeyewindowgridshapedataitem.h"

#include <QUndoCommand>
#include <memory>

class Post2dBirdEyeWindowGridShapeDataItem::UpdateActorSettingsCommand : public QUndoCommand
{
public:
	UpdateActorSettingsCommand(bool apply, QUndoCommand* command, Post2dBirdEyeWindowGridShapeDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;
	std::unique_ptr<QUndoCommand> m_command;
	Post2dBirdEyeWindowGridShapeDataItem* m_item;
};

#endif // POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
