#ifndef GEODATAPOLYLINE_EDITPROPERTYCOMMAND_H
#define GEODATAPOLYLINE_EDITPROPERTYCOMMAND_H

#include "../geodatapolyline.h"

#include <QUndoCommand>

class GeoDataPolyLine::EditPropertyCommand : public QUndoCommand
{
public:
	EditPropertyCommand(const GeoDataPolyLineColorSettingDialog::Setting& s, GeoDataPolyLine* p);

	void redo() override;
	void undo() override;

private:
	GeoDataPolyLineColorSettingDialog::Setting m_newSetting;
	GeoDataPolyLineColorSettingDialog::Setting m_oldSetting;
	GeoDataPolyLine* m_polyline;
};

#endif // GEODATAPOLYLINE_EDITPROPERTYCOMMAND_H
