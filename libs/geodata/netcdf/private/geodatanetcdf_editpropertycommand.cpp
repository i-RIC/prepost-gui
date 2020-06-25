#include "geodatanetcdf_editpropertycommand.h"

GeoDataNetcdf::EditPropertyCommand::EditPropertyCommand(const GeoDataNetcdfColorSettingDialog::Setting& s, GeoDataNetcdf* n) :
	QUndoCommand {GeoDataNetcdf::tr("Color setting edit")},
	m_newSetting {s},
	m_oldSetting {n->m_colorSetting},
	m_netcdf {n}
{}

void GeoDataNetcdf::EditPropertyCommand::redo()
{
	m_netcdf->m_colorSetting = m_newSetting;
	m_netcdf->updateActorSetting();
}

void GeoDataNetcdf::EditPropertyCommand::undo()
{
	m_netcdf->m_colorSetting = m_oldSetting;
	m_netcdf->updateActorSetting();
}
