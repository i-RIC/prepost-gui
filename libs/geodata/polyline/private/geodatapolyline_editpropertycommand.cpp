#include "geodatapolyline_editpropertycommand.h"

GeoDataPolyLine::EditPropertyCommand::EditPropertyCommand(const GeoDataPolyLineColorSimpleSettingDialog::Setting& s, GeoDataPolyLine* p) :
	QUndoCommand {GeoDataPolyLine::tr("PolyLine property edit")},
	m_newSetting {s},
	m_oldSetting {p->colorSetting()},
	m_polyline {p}
{}

void GeoDataPolyLine::EditPropertyCommand::redo()
{
	m_polyline->setColorSetting(m_newSetting);
	m_polyline->updateActorSettings();
}

void GeoDataPolyLine::EditPropertyCommand::undo()
{
	m_polyline->setColorSetting(m_oldSetting);
	m_polyline->updateActorSettings();
}
