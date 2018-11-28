#include "geodatapolyline_editvaluecommand.h"

GeoDataPolyLine::EditValueCommand::EditValueCommand(const QVariant& newvalue, GeoDataPolyLine* polyline) :
	QUndoCommand {GeoDataPolyLine::tr("PolyLine value change")},
	m_newValue {newvalue},
	m_oldValue {polyline->variantValue()},
	m_oldMapped {polyline->isMapped()},
	m_polyline {polyline}
{}

void GeoDataPolyLine::EditValueCommand::redo()
{
	m_polyline->setVariantValue(m_newValue);
	m_polyline->setMapped(false);
}

void GeoDataPolyLine::EditValueCommand::undo()
{
	m_polyline->setVariantValue(m_oldValue);
	m_polyline->setMapped(m_oldMapped);
}
