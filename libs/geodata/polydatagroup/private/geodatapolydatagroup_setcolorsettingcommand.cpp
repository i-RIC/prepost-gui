#include "geodatapolydatagroup_setcolorsettingcommand.h"

GeoDataPolyDataGroup::SetColorSettingCommand::SetColorSettingCommand(const GeoDataPolyDataGroupColorSettingDialog::Setting& s, GeoDataPolyDataGroup* g) :
	QUndoCommand {GeoDataPolyDataGroup::tr("Color Setting Edit")},
	m_newSetting {s},
	m_oldSetting {g->colorSetting()},
	m_group {g}
{}

void GeoDataPolyDataGroup::SetColorSettingCommand::redo()
{
	m_group->setColorSetting(m_newSetting);
}

void GeoDataPolyDataGroup::SetColorSettingCommand::undo()
{
	m_group->setColorSetting(m_oldSetting);
}
