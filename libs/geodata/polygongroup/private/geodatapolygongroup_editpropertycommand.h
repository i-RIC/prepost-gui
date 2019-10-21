#ifndef GEODATAPOLYGONGROUP_EDITPROPERTYCOMMAND_H
#define GEODATAPOLYGONGROUP_EDITPROPERTYCOMMAND_H

#include "../geodatapolygongroup.h"

#include <QUndoCommand>

class GeoDataPolygonGroup::EditPropertyCommand : public QUndoCommand
{
public:
	EditPropertyCommand(const GeoDataPolygonGroupColorSettingDialog::Setting& s, GeoDataPolygonGroup* p);

	void redo() override;
	void undo() override;

private:
	GeoDataPolygonGroupColorSettingDialog::Setting m_newSetting;
	GeoDataPolygonGroupColorSettingDialog::Setting m_oldSetting;
	GeoDataPolygonGroup* m_group;
};

#endif // GEODATAPOLYGONGROUP_EDITPROPERTYCOMMAND_H
