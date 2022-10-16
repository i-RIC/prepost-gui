#ifndef GEODATAPOLYGONGROUPPROXY_SETSETTINGCOMMAND_H
#define GEODATAPOLYGONGROUPPROXY_SETSETTINGCOMMAND_H

#include "../geodatapolygongroupproxy.h"

#include <QUndoCommand>

class GeoDataPolygonGroupProxy::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const GeoDataPolygonGroupColorSettingDialog::Setting& setting, GeoDataPolygonGroupProxy* proxy);

	void redo() override;
	void undo() override;

private:
	GeoDataPolygonGroupColorSettingDialog::Setting m_newSetting;
	GeoDataPolygonGroupColorSettingDialog::Setting m_oldSetting;
	GeoDataPolygonGroupProxy* m_proxy;
};

#endif // GEODATAPOLYGONGROUPPROXY_SETSETTINGCOMMAND_H
