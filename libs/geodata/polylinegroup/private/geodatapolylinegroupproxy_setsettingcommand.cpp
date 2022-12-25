#include "geodatapolylinegroupproxy_impl.h"
#include "geodatapolylinegroupproxy_setsettingcommand.h"

GeoDataPolyLineGroupProxy::SetSettingCommand::SetSettingCommand(const GeoDataPolyLineGroupColorSettingDialog::Setting& setting, GeoDataPolyLineGroupProxy* proxy) :
	QUndoCommand(GeoDataPolyLineGroupProxy::tr("Edit display setting")),
	m_newSetting {setting},
	m_oldSetting {proxy->impl->m_setting},
	m_proxy {proxy}
{}

void GeoDataPolyLineGroupProxy::SetSettingCommand::redo()
{
	m_proxy->impl->m_setting = m_newSetting;
	m_proxy->updateGraphics();
}

void GeoDataPolyLineGroupProxy::SetSettingCommand::undo()
{
	m_proxy->impl->m_setting = m_oldSetting;
	m_proxy->updateGraphics();
}
