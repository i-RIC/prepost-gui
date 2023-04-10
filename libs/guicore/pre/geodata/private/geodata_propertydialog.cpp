#include "geodata_propertydialog.h"
#include "geodata_updateactorsettingcommand.h"

GeoData::PropertyDialog::PropertyDialog(GeoData* geoData, QWidget* parent) :
	ModifyCommandDialog {parent},
	m_geoData {geoData}
{}

void GeoData::PropertyDialog::pushCommand(QUndoCommand* command)
{
	m_geoData->pushRenderCommand(new UpdateActorSettingCommand(command, m_geoData));
}
