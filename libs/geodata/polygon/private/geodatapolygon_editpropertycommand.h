#ifndef GEODATAPOLYGON_EDITPROPERTYCOMMAND_H
#define GEODATAPOLYGON_EDITPROPERTYCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>

class GeoDataPolygon::EditPropertyCommand : public QUndoCommand
{
public:
	EditPropertyCommand(const GeoDataPolygonColorSettingDialog::Setting& s, GeoDataPolygon* p);

	void redo() override;
	void undo() override;

private:
	GeoDataPolygonColorSettingDialog::Setting m_newSetting;
	GeoDataPolygonColorSettingDialog::Setting m_oldSetting;
	GeoDataPolygon* m_polygon;
};

#endif // GEODATAPOLYGON_EDITPROPERTYCOMMAND_H
