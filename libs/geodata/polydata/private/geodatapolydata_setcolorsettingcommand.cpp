#include "geodatapolydata_setcolorsettingcommand.h"

GeoDataPolyData::SetColorSettingCommand::SetColorSettingCommand(const GeoDataPolyDataColorSettingDialog::Setting& s, GeoDataPolyData* p) :
	QUndoCommand {GeoDataPolyData::tr("Edit color setting")},
	m_newSetting {s},
	m_oldSetting {p->colorSetting()},
	m_polyData {p}
{}

void GeoDataPolyData::SetColorSettingCommand::redo()
{
	m_polyData->setColorSetting(m_newSetting);
	m_polyData->updateActorSettings();
}

void GeoDataPolyData::SetColorSettingCommand::undo()
{
	m_polyData->setColorSetting(m_oldSetting);
	m_polyData->updateActorSettings();
}
