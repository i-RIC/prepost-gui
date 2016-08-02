#include "distancemeasuredataitem_redefinecommand.h"

DistanceMeasureDataItem::RedefineCommand::RedefineCommand(DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Redefine Distance Measure")),
	m_item {item}
{}

void DistanceMeasureDataItem::RedefineCommand::redo()
{
	m_item->m_mouseEventMode = DistanceMeasureDataItem::meBeforeDefining;
	m_item->updateMouseCursor();

	m_item->m_defined = false;
	m_item->updateActorSettings();
}

void DistanceMeasureDataItem::RedefineCommand::undo()
{
	m_item->m_mouseEventMode = DistanceMeasureDataItem::meNormal;
	m_item->updateMouseCursor();

	m_item->m_defined = true;
	m_item->updateActorSettings();
}
