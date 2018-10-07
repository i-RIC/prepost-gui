#include "geodatapolygon_editpropertycommand.h"

GeoDataPolygon::EditPropertyCommand::EditPropertyCommand(const GeoDataPolygonColorSettingDialog::Setting& s, GeoDataPolygon* p) :
	QUndoCommand {GeoDataPolygon::tr("Polygon property edit")},
	m_newSetting {s},
	m_oldSetting {p->colorSetting()},
	m_polygon {p}
{}

void GeoDataPolygon::EditPropertyCommand::redo()
{
	m_polygon->setColorSetting(m_newSetting);
	m_polygon->updateActorSettings();
}

void GeoDataPolygon::EditPropertyCommand::undo()
{
	m_polygon->setColorSetting(m_oldSetting);
	m_polygon->updateActorSettings();
}
