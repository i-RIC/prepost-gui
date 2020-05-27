#include "geodatapolydata_editnameandvaluecommand.h"

GeoDataPolyData::EditNameAndValueCommand::EditNameAndValueCommand(const QString& newName, const QVariant newValue, GeoDataPolyData* polyData) :
	QUndoCommand {GeoDataPolyData::tr("%1 name and value change").arg(polyData->shapeNameCamelCase())},
	m_newName {newName},
	m_newValue {newValue},
	m_oldName {polyData->caption()},
	m_oldValue {polyData->variantValue()},
	m_oldMapped {polyData->isMapped()},
	m_polyData {polyData}
{}

void GeoDataPolyData::EditNameAndValueCommand::redo()
{
	m_polyData->setCaption(m_newName);
	m_polyData->setVariantValue(m_newValue);
	m_polyData->setMapped(false);
	m_polyData->emitNameAndValueEdited();
}

void GeoDataPolyData::EditNameAndValueCommand::undo()
{
	m_polyData->setCaption(m_oldName);
	m_polyData->setVariantValue(m_oldValue);
	m_polyData->setMapped(m_oldMapped);
	m_polyData->emitNameAndValueEdited();
}
