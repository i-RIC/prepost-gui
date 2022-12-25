#ifndef GEODATAPOLYGONGROUP_SETCOLORSETTINGCOMMAND_H
#define GEODATAPOLYGONGROUP_SETCOLORSETTINGCOMMAND_H

#include "../geodatapolygongroup.h"
#include "../geodatapolygongroupcolorsettingdialog.h"

#include <QUndoCommand>

class GeoDataPolygonGroup::SetColorSettingCommand : public QUndoCommand
{
public:
	SetColorSettingCommand(const GeoDataPolygonGroupColorSettingDialog::Setting& setting, GeoDataPolygonGroup* group);

	void redo() override;
	void undo() override;

private:
	GeoDataPolygonGroupColorSettingDialog::Setting m_newSetting;
	GeoDataPolygonGroupColorSettingDialog::Setting m_oldSetting;
	GeoDataPolygonGroup* m_group;
};

#endif // GEODATAPOLYGONGROUP_SETCOLORSETTINGCOMMAND_H
