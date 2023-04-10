#include "post2dwindowgridtypedataitem_applycolormapsettingcommand.h"


Post2dWindowGridTypeDataItem::ApplyColorMapSettingCommand::ApplyColorMapSettingCommand(QUndoCommand* command, const std::string& name, bool apply, Post2dWindowGridTypeDataItem* item) :
	QUndoCommand {},
	m_command {command},
	m_name {name},
	m_apply {apply},
	m_item {item}
{}

void Post2dWindowGridTypeDataItem::ApplyColorMapSettingCommand::redo()
{
	m_command->redo();
	m_item->applyColorMapSetting(m_name);
}

void Post2dWindowGridTypeDataItem::ApplyColorMapSettingCommand::undo()
{
	m_command->undo();
	m_item->applyColorMapSetting(m_name);
}

int Post2dWindowGridTypeDataItem::ApplyColorMapSettingCommand::id() const
{
	return m_command->id();
}

bool Post2dWindowGridTypeDataItem::ApplyColorMapSettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto c = dynamic_cast<const ApplyColorMapSettingCommand*>(other);
	if (c == nullptr) {return false;}

	if (m_item != c->m_item) {return false;}
	if (m_name != c->m_name) {return false;}

	m_apply = c->m_apply;
	return m_command->mergeWith(c->m_command.get());
}
