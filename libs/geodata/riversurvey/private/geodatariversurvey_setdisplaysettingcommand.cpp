#include "geodatariversurvey_setdisplaysettingcommand.h"

GeoDataRiverSurvey::SetDisplaySettingCommand::SetDisplaySettingCommand(const GeoDataRiverSurveyDisplaySetting& setting, GeoDataRiverSurvey* s) :
	QUndoCommand {GeoDataRiverSurvey::tr("Edit Display Setting")},
	m_newSetting {setting},
	m_oldSetting {s->m_setting},
	m_survey {s}
{}

void GeoDataRiverSurvey::SetDisplaySettingCommand::redo()
{
	m_survey->m_setting = m_newSetting;
	m_survey->updateShapeData();
}

void GeoDataRiverSurvey::SetDisplaySettingCommand::undo()
{
	m_survey->m_setting = m_oldSetting;
	m_survey->updateShapeData();
}
