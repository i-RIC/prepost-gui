#include "preprocessorgridtypedataitem_applycolormapsettingandrendercommand.h"

PreProcessorGridTypeDataItem::ApplyColorMapSettingAndRenderCommand::ApplyColorMapSettingAndRenderCommand(QUndoCommand* command, const std::string& name, bool apply, PreProcessorGridTypeDataItem* item) :
	QUndoCommand {},
	m_command {command},
	m_name {name},
	m_apply {apply},
	m_item {item}
{}

void PreProcessorGridTypeDataItem::ApplyColorMapSettingAndRenderCommand::redo()
{
	m_command->redo();
	m_item->applyColorMapSetting(m_name);
	m_item->renderGraphicsView();
}

void PreProcessorGridTypeDataItem::ApplyColorMapSettingAndRenderCommand::undo()
{
	m_command->undo();
	m_item->applyColorMapSetting(m_name);
	m_item->renderGraphicsView();
}

int PreProcessorGridTypeDataItem::ApplyColorMapSettingAndRenderCommand::id() const
{
	return m_command->id();
}

bool PreProcessorGridTypeDataItem::ApplyColorMapSettingAndRenderCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto c = dynamic_cast<const ApplyColorMapSettingAndRenderCommand*>(other);
	if (c == nullptr) {return false;}

	if (m_item != c->m_item) {return false;}
	if (m_name != c->m_name) {return false;}

	m_apply = c->m_apply;
	return m_command->mergeWith(c->m_command.get());
}
