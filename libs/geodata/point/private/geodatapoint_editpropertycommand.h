#ifndef GEODATAPOINT_EDITPROPERTYCOMMAND_H
#define GEODATAPOINT_EDITPROPERTYCOMMAND_H

#include "../geodatapoint.h"

#include <QUndoCommand>

class GeoDataPoint::EditPropertyCommand : public QUndoCommand
{
public:
	EditPropertyCommand(const GeoDataPointColorSimpleSettingDialog::Setting& s, GeoDataPoint* p);

	void redo() override;
	void undo() override;

private:
	GeoDataPointColorSimpleSettingDialog::Setting m_newSetting;
	GeoDataPointColorSimpleSettingDialog::Setting m_oldSetting;
	GeoDataPoint* m_point;
};

#endif // GEODATAPOINT_EDITPROPERTYCOMMAND_H
