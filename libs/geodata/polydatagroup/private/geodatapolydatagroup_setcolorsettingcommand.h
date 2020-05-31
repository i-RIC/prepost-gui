#ifndef GEODATAPOLYDATAGROUP_SETCOLORSETTINGCOMMAND_H
#define GEODATAPOLYDATAGROUP_SETCOLORSETTINGCOMMAND_H

#include "../geodatapolydatagroup.h"

#include <QUndoCommand>

class GeoDataPolyDataGroup::SetColorSettingCommand : public QUndoCommand
{
public:
	SetColorSettingCommand(const GeoDataPolyDataGroupColorSettingDialog::Setting& s, GeoDataPolyDataGroup* p);

	void redo() override;
	void undo() override;

private:
	GeoDataPolyDataGroupColorSettingDialog::Setting m_newSetting;
	GeoDataPolyDataGroupColorSettingDialog::Setting m_oldSetting;
	GeoDataPolyDataGroup* m_group;
};

#endif // GEODATAPOLYDATAGROUP_SETCOLORSETTINGCOMMAND_H
