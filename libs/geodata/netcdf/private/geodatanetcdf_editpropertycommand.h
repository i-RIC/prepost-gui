#ifndef GEODATANETCDF_EDITPROPERTYCOMMAND_H
#define GEODATANETCDF_EDITPROPERTYCOMMAND_H

#include "../geodatanetcdf.h"

#include <QUndoCommand>

class GeoDataNetcdf::EditPropertyCommand : public QUndoCommand
{
public:
	EditPropertyCommand(const GeoDataNetcdfColorSettingDialog::Setting& s, GeoDataNetcdf* n);

	void redo() override;
	void undo() override;

private:
	GeoDataNetcdfColorSettingDialog::Setting m_newSetting;
	GeoDataNetcdfColorSettingDialog::Setting m_oldSetting;
	GeoDataNetcdf* m_netcdf;
};

#endif // GEODATANETCDF_EDITPROPERTYCOMMAND_H
