#ifndef POST2DWINDOWGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGANDRENDERCOMMAND_H
#define POST2DWINDOWGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGANDRENDERCOMMAND_H

#include "../post2dwindowgridtypedataitem.h"

#include <QUndoCommand>

#include <memory>

class Post2dWindowGridTypeDataItem::ApplyColorMapSettingAndRenderCommand : public QUndoCommand
{
public:
	ApplyColorMapSettingAndRenderCommand(QUndoCommand* command, const std::string& name, bool apply, Post2dWindowGridTypeDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	std::string m_name;
	bool m_apply;
	Post2dWindowGridTypeDataItem* m_item;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGANDRENDERCOMMAND_H
