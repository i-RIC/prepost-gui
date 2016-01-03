#include "geodatariversurveyproxy_setsettingcommand.h"

GeoDataRiverSurveyProxy::SetSettingCommand::SetSettingCommand(const GeoDataRiverSurveyDisplaySetting& setting, GeoDataRiverSurveyProxy* proxy) :
	QUndoCommand(GeoDataRiverSurveyProxy::tr("Edit display setting")),
	m_newSetting {setting},
	m_oldSetting {proxy->m_setting},
	m_proxy {proxy}
{}

void GeoDataRiverSurveyProxy::SetSettingCommand::redo()
{
	m_proxy->m_setting = m_newSetting;
	m_proxy->updateGraphics();
}

void GeoDataRiverSurveyProxy::SetSettingCommand::undo()
{
	m_proxy->m_setting = m_oldSetting;
	m_proxy->updateGraphics();
}
