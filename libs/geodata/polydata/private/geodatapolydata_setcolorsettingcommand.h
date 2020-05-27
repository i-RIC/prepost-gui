#ifndef GEODATAPOLYDATA_SETCOLORSETTINGCOMMAND_H
#define GEODATAPOLYDATA_SETCOLORSETTINGCOMMAND_H

#include "../geodatapolydata.h"

#include <QUndoCommand>

class GeoDataPolyData::SetColorSettingCommand : public QUndoCommand
{
public:
	SetColorSettingCommand(const GeoDataPolyDataColorSettingDialog::Setting& s, GeoDataPolyData* p);

	void redo() override;
	void undo() override;

private:
	GeoDataPolyDataColorSettingDialog::Setting m_newSetting;
	GeoDataPolyDataColorSettingDialog::Setting m_oldSetting;
	GeoDataPolyData* m_polyData;
};

#endif // GEODATAPOLYDATA_SETCOLORSETTINGCOMMAND_H
