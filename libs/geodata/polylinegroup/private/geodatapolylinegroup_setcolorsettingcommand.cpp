#include "geodatapolylinegroup_impl.h"
#include "geodatapolylinegroup_setcolorsettingcommand.h"

GeoDataPolyLineGroup::SetColorSettingCommand::SetColorSettingCommand(const GeoDataPolyLineGroupColorSettingDialog::Setting& setting, GeoDataPolyLineGroup* group) :
	QUndoCommand {GeoDataPolyLineGroup::tr("Color Setting Edit")},
	m_newSetting {setting},
	m_oldSetting {group->impl->m_colorSetting},
	m_group {group}
{}

void GeoDataPolyLineGroup::SetColorSettingCommand::redo()
{
	m_group->impl->m_colorSetting = m_newSetting;
	m_group->updateActorSetting();
}

void GeoDataPolyLineGroup::SetColorSettingCommand::undo()
{
	m_group->impl->m_colorSetting = m_oldSetting;
	m_group->updateActorSetting();
}
