#include "geodatapolygongroup_impl.h"
#include "geodatapolygongroup_setcolorsettingcommand.h"

GeoDataPolygonGroup::SetColorSettingCommand::SetColorSettingCommand(const GeoDataPolygonGroupColorSettingDialog::Setting& setting, GeoDataPolygonGroup* group) :
	QUndoCommand {GeoDataPolygonGroup::tr("Color Setting Edit")},
	m_newSetting {setting},
	m_oldSetting {group->impl->m_colorSetting},
	m_group {group}
{}

void GeoDataPolygonGroup::SetColorSettingCommand::redo()
{
	m_group->impl->m_colorSetting = m_newSetting;
	m_group->updateActorSetting();
}

void GeoDataPolygonGroup::SetColorSettingCommand::undo()
{
	m_group->impl->m_colorSetting = m_oldSetting;
	m_group->updateActorSetting();
}
