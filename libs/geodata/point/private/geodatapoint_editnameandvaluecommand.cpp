#include "geodatapoint_editnameandvaluecommand.h"

GeoDataPoint::EditNameAndValueCommand::EditNameAndValueCommand(const QString& newName, const QVariant newValue, GeoDataPoint* point) :
	QUndoCommand {GeoDataPoint::tr("Poly line name and value change")},
	m_newName {newName},
	m_newValue {newValue},
	m_oldName {point->caption()},
	m_oldValue {point->variantValue()},
	m_oldMapped {point->isMapped()},
	m_point {point}
{}

void GeoDataPoint::EditNameAndValueCommand::redo()
{
	m_point->setCaption(m_newName);
	m_point->setVariantValue(m_newValue);
	m_point->setMapped(false);
	m_point->emitNameAndValueEdited();
}

void GeoDataPoint::EditNameAndValueCommand::undo()
{
	m_point->setCaption(m_oldName);
	m_point->setVariantValue(m_oldValue);
	m_point->setMapped(m_oldMapped);
	m_point->emitNameAndValueEdited();
}
