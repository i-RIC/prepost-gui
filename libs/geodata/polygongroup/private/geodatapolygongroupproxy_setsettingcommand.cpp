#include "geodatapolygongroupproxy.h"
#include "geodatapolygongroupproxy_impl.h"
#include "geodatapolygongroupproxy_setsettingcommand.h"

GeoDataPolygonGroupProxy::SetSettingCommand::SetSettingCommand(const GeoDataPolygonGroupColorSettingDialog::Setting& setting, GeoDataPolygonGroupProxy* proxy) :
	QUndoCommand(GeoDataPolygonGroupProxy::tr("Edit display setting")),
	m_newSetting {setting},
	m_oldSetting {proxy->impl->m_setting},
	m_proxy {proxy}
{}

void GeoDataPolygonGroupProxy::SetSettingCommand::redo()
{
	m_proxy->impl->m_setting = m_newSetting;
	m_proxy->updateGraphics();
}

void GeoDataPolygonGroupProxy::SetSettingCommand::undo()
{
	m_proxy->impl->m_setting = m_oldSetting;
	m_proxy->updateGraphics();
}
