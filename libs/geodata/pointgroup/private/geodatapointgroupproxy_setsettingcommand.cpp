#include "geodatapointgroupproxy_impl.h"
#include "geodatapointgroupproxy_setsettingcommand.h"

GeoDataPointGroupProxy::SetSettingCommand::SetSettingCommand(const GeoDataPointGroupColorSettingDialog::Setting& setting, GeoDataPointGroupProxy* proxy) :
	QUndoCommand(GeoDataPointGroupProxy::tr("Edit display setting")),
	m_newSetting {setting},
	m_oldSetting {proxy->impl->m_setting},
	m_proxy {proxy}
{}

void GeoDataPointGroupProxy::SetSettingCommand::redo()
{
	m_proxy->impl->m_setting = m_newSetting;
	m_proxy->updateGraphics();
}

void GeoDataPointGroupProxy::SetSettingCommand::undo()
{
	m_proxy->impl->m_setting = m_oldSetting;
	m_proxy->updateGraphics();
}
