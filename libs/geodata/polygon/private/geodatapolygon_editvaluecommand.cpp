#include "geodatapolygon_editvaluecommand.h"

GeoDataPolygon::EditValueCommand::EditValueCommand(const QVariant& newvalue, GeoDataPolygon* polygon) :
	QUndoCommand {GeoDataPolygon::tr("Polygon value change")},
	m_newValue {newvalue},
	m_oldValue {polygon->variantValue()},
	m_oldMapped {polygon->isMapped()},
	m_polygon {polygon}
{}

void GeoDataPolygon::EditValueCommand::redo()
{
	m_polygon->setVariantValue(m_newValue);
	m_polygon->setMapped(false);
}

void GeoDataPolygon::EditValueCommand::undo()
{
	m_polygon->setVariantValue(m_oldValue);
	m_polygon->setMapped(m_oldMapped);
}
