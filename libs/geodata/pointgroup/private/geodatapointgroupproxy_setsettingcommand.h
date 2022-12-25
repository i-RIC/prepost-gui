#ifndef GEODATAPOINTGROUPPROXY_SETSETTINGCOMMAND_H
#define GEODATAPOINTGROUPPROXY_SETSETTINGCOMMAND_H

#include "../geodatapointgroupproxy.h"

#include <QUndoCommand>

class GeoDataPointGroupProxy::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const GeoDataPointGroupColorSettingDialog::Setting& setting, GeoDataPointGroupProxy* proxy);

	void redo() override;
	void undo() override;

private:
	GeoDataPointGroupColorSettingDialog::Setting m_newSetting;
	GeoDataPointGroupColorSettingDialog::Setting m_oldSetting;
	GeoDataPointGroupProxy* m_proxy;
};

#endif // GEODATAPOINTGROUPPROXY_SETSETTINGCOMMAND_H
