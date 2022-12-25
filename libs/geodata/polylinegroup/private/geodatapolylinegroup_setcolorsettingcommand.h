#ifndef GEODATAPOLYLINEGROUP_SETCOLORSETTINGCOMMAND_H
#define GEODATAPOLYLINEGROUP_SETCOLORSETTINGCOMMAND_H

#include "../geodatapolylinegroup.h"
#include "../geodatapolylinegroupcolorsettingdialog.h"

#include <QUndoCommand>

class GeoDataPolyLineGroup::SetColorSettingCommand : public QUndoCommand
{
public:
	SetColorSettingCommand(const GeoDataPolyLineGroupColorSettingDialog::Setting& setting, GeoDataPolyLineGroup* group);

	void redo() override;
	void undo() override;

private:
	GeoDataPolyLineGroupColorSettingDialog::Setting m_newSetting;
	GeoDataPolyLineGroupColorSettingDialog::Setting m_oldSetting;
	GeoDataPolyLineGroup* m_group;
};

#endif // GEODATAPOLYLINEGROUP_SETCOLORSETTINGCOMMAND_H
