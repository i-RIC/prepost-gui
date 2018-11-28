#ifndef GEODATAPOLYLINE_EDITPROPERTYCOMMAND_H
#define GEODATAPOLYLINE_EDITPROPERTYCOMMAND_H

#include "../geodatapolyline.h"

#include <QUndoCommand>

class GeoDataPolyLine::EditPropertyCommand : public QUndoCommand
{
public:
	EditPropertyCommand(const GeoDataPolyLineColorSimpleSettingDialog::Setting& s, GeoDataPolyLine* p);

	void redo() override;
	void undo() override;

private:
	GeoDataPolyLineColorSimpleSettingDialog::Setting m_newSetting;
	GeoDataPolyLineColorSimpleSettingDialog::Setting m_oldSetting;
	GeoDataPolyLine* m_polyline;
};

#endif // GEODATAPOLYLINE_EDITPROPERTYCOMMAND_H
