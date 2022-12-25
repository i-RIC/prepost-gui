#ifndef GEODATAPOINTGROUP_SETCOLORSETTINGCOMMAND_H
#define GEODATAPOINTGROUP_SETCOLORSETTINGCOMMAND_H

#include "../geodatapointgroup.h"
#include "../geodatapointgroupcolorsettingdialog.h"

#include <QUndoCommand>

class GeoDataPointGroup::SetColorSettingCommand : public QUndoCommand
{
public:
	SetColorSettingCommand(const GeoDataPointGroupColorSettingDialog::Setting& setting, GeoDataPointGroup* group);

	void redo() override;
	void undo() override;

private:
	GeoDataPointGroupColorSettingDialog::Setting m_newSetting;
	GeoDataPointGroupColorSettingDialog::Setting m_oldSetting;
	GeoDataPointGroup* m_group;
};

#endif // GEODATAPOINTGROUP_SETCOLORSETTINGCOMMAND_H
