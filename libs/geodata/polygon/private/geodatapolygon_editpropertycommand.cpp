#include "geodatapolygon_editpropertycommand.h"

GeoDataPolygon::EditPropertyCommand::EditPropertyCommand(const GeoDataPolygonColorSettingDialog::Setting& s, GeoDataPolygon* p) :
	QUndoCommand {GeoDataPolygon::tr("Polygon property edit")},
	m_newSetting {s},
	m_oldSetting {p->m_setting},
	m_polygon {p}
{}

void GeoDataPolygon::EditPropertyCommand::redo()
{
	m_polygon->m_setting = m_newSetting;
	m_polygon->updateActorSettings();
}

void GeoDataPolygon::EditPropertyCommand::undo()
{
	m_polygon->m_setting = m_oldSetting;
	m_polygon->updateActorSettings();
}
