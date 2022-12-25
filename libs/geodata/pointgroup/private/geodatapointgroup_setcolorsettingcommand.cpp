#include "geodatapointgroup_impl.h"
#include "geodatapointgroup_setcolorsettingcommand.h"

GeoDataPointGroup::SetColorSettingCommand::SetColorSettingCommand(const GeoDataPointGroupColorSettingDialog::Setting& setting, GeoDataPointGroup* group) :
	QUndoCommand {GeoDataPointGroup::tr("Color Setting Edit")},
	m_newSetting {setting},
	m_oldSetting {group->impl->m_colorSetting},
	m_group {group}
{}

void GeoDataPointGroup::SetColorSettingCommand::redo()
{
	m_group->impl->m_colorSetting = m_newSetting;
	m_group->updateActorSetting();
}

void GeoDataPointGroup::SetColorSettingCommand::undo()
{
	m_group->impl->m_colorSetting = m_oldSetting;
	m_group->updateActorSetting();
}
