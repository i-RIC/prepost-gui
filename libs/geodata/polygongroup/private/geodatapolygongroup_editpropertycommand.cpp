#include "geodatapolygongroup_editpropertycommand.h"

GeoDataPolygonGroup::EditPropertyCommand::EditPropertyCommand(const GeoDataPolygonGroupColorSettingDialog::Setting& s, GeoDataPolygonGroup* g) :
	QUndoCommand {GeoDataPolygonGroup::tr("Polygon property edit")},
	m_newSetting {s},
	m_oldSetting {g->colorSetting()},
	m_group {g}
{}

void GeoDataPolygonGroup::EditPropertyCommand::redo()
{
	m_group->setColorSetting(m_newSetting);
}

void GeoDataPolygonGroup::EditPropertyCommand::undo()
{
	m_group->setColorSetting(m_oldSetting);
}
