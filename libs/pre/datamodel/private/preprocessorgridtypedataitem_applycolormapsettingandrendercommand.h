#ifndef PREPROCESSORGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGANDRENDERCOMMAND_H
#define PREPROCESSORGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGANDRENDERCOMMAND_H

#include "../preprocessorgridtypedataitem.h"

#include <QUndoCommand>

#include <memory>

class PreProcessorGridTypeDataItem::ApplyColorMapSettingAndRenderCommand : public QUndoCommand
{
public:
	ApplyColorMapSettingAndRenderCommand(QUndoCommand* command, const std::string& name, bool apply, PreProcessorGridTypeDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	std::string m_name;
	bool m_apply;
	PreProcessorGridTypeDataItem* m_item;
};

#endif // PREPROCESSORGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGANDRENDERCOMMAND_H
