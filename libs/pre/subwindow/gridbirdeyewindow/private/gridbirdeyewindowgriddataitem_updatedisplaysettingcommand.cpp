#include "gridbirdeyewindowgriddataitem_updatedisplaysettingcommand.h"

#include <misc/qundocommandhelper.h>

GridBirdEyeWindowGridDataItem::UpdateDisplaySettingCommand::UpdateDisplaySettingCommand(bool apply, QUndoCommand* s, QUndoCommand* cs) :
	m_apply {apply},
	m_settingCommand {s},
	m_colormapSettingCommand {cs}
{}

GridBirdEyeWindowGridDataItem::UpdateDisplaySettingCommand::~UpdateDisplaySettingCommand()
{
	delete m_settingCommand;
	delete m_colormapSettingCommand;
}

void GridBirdEyeWindowGridDataItem::UpdateDisplaySettingCommand::redo()
{
	m_settingCommand->redo();
	if (m_colormapSettingCommand != nullptr) {
		m_colormapSettingCommand->redo();
	}
}

void GridBirdEyeWindowGridDataItem::UpdateDisplaySettingCommand::undo()
{
	m_settingCommand->undo();
	if (m_colormapSettingCommand != nullptr) {
		m_colormapSettingCommand->undo();
	}
}

int GridBirdEyeWindowGridDataItem::UpdateDisplaySettingCommand::id() const
{
	return iRIC::generateCommandId("GridBirdEyeWindowGridDataItem::UpdateDisplaySettingCommand");
}

bool GridBirdEyeWindowGridDataItem::UpdateDisplaySettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = const_cast<QUndoCommand*> (other);
	auto c = dynamic_cast<UpdateDisplaySettingCommand*> (other2);
	if (c == nullptr) {return false;}

	m_settingCommand->mergeWith(c->m_settingCommand);

	if (m_colormapSettingCommand == nullptr) {
		m_colormapSettingCommand = c->m_colormapSettingCommand;
		c->m_colormapSettingCommand = nullptr;
	} else if (c->m_colormapSettingCommand == nullptr) {
		m_colormapSettingCommand->undo();
		delete m_colormapSettingCommand;
		m_colormapSettingCommand = nullptr;
	} else {
		bool ok = m_colormapSettingCommand->mergeWith(c->m_colormapSettingCommand);

		if (!ok) {
			m_colormapSettingCommand->undo();
			delete m_colormapSettingCommand;
			m_colormapSettingCommand = c->m_colormapSettingCommand;
			c->m_colormapSettingCommand = nullptr;
		}
	}
	m_apply = c->m_apply;
	return true;
}
