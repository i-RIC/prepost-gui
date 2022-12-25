#ifndef GEODATAPOLYLINEGROUPPROXY_SETSETTINGCOMMAND_H
#define GEODATAPOLYLINEGROUPPROXY_SETSETTINGCOMMAND_H

#include "../geodatapolylinegroupproxy.h"

#include <QUndoCommand>

class GeoDataPolyLineGroupProxy::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const GeoDataPolyLineGroupColorSettingDialog::Setting& setting, GeoDataPolyLineGroupProxy* proxy);

	void redo() override;
	void undo() override;

private:
	GeoDataPolyLineGroupColorSettingDialog::Setting m_newSetting;
	GeoDataPolyLineGroupColorSettingDialog::Setting m_oldSetting;
	GeoDataPolyLineGroupProxy* m_proxy;
};

#endif // GEODATAPOLYLINEGROUPPROXY_SETSETTINGCOMMAND_H
