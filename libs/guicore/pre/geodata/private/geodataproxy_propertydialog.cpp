#include "geodataproxy_propertydialog.h"
#include "geodataproxy_updateactorsettingcommand.h"

GeoDataProxy::PropertyDialog::PropertyDialog(GeoDataProxy* proxy, QWidget* parent) :
	ModifyCommandDialog {parent},
	m_proxy {proxy}
{}

void GeoDataProxy::PropertyDialog::pushCommand(QUndoCommand* command)
{
	m_proxy->pushRenderCommand(new UpdateActorSettingCommand(command, m_proxy));
}
