#include "geodatapolygon_editnameandvaluecommand.h"

GeoDataPolygon::EditNameAndValueCommand::EditNameAndValueCommand(const QString& newName, const QVariant newValue, GeoDataPolygon* polygon) :
	QUndoCommand {GeoDataPolygon::tr("Polygon name and value change")},
	m_newName {newName},
	m_newValue {newValue},
	m_oldName {polygon->caption()},
	m_oldValue {polygon->variantValue()},
	m_oldMapped {polygon->isMapped()},
	m_polygon {polygon}
{}

void GeoDataPolygon::EditNameAndValueCommand::redo()
{
	m_polygon->setCaption(m_newName);
	m_polygon->setVariantValue(m_newValue);
	m_polygon->setMapped(false);
	m_polygon->emitNameAndValueEdited();
}

void GeoDataPolygon::EditNameAndValueCommand::undo()
{
	m_polygon->setCaption(m_oldName);
	m_polygon->setVariantValue(m_oldValue);
	m_polygon->setMapped(m_oldMapped);
	m_polygon->emitNameAndValueEdited();
}
