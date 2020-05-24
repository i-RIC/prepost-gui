#include "geodatapoint_editpropertycommand.h"

GeoDataPoint::EditPropertyCommand::EditPropertyCommand(const GeoDataPointColorSimpleSettingDialog::Setting& s, GeoDataPoint* p) :
	QUndoCommand {GeoDataPoint::tr("Point property edit")},
	m_newSetting {s},
	m_oldSetting {p->colorSetting()},
	m_point {p}
{}

void GeoDataPoint::EditPropertyCommand::redo()
{
	m_point->setColorSetting(m_newSetting);
	m_point->updateActorSettings();
}

void GeoDataPoint::EditPropertyCommand::undo()
{
	m_point->setColorSetting(m_oldSetting);
	m_point->updateActorSettings();
}
