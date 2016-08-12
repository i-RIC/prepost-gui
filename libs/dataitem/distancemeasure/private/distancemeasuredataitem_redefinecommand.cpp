#include "distancemeasuredataitem_redefinecommand.h"

DistanceMeasureDataItem::RedefineCommand::RedefineCommand(DistanceMeasureDataItem* item) :
	QUndoCommand(DistanceMeasureDataItem::tr("Redefine Distance Measure")),
	m_item {item}
{}

void DistanceMeasureDataItem::RedefineCommand::redo()
{
	m_item->impl->m_mouseEventMode = Impl::meBeforeDefining;
	m_item->updateMouseCursor();

	m_item->impl->m_setting.defined = false;
	m_item->updateActorSettings();
}

void DistanceMeasureDataItem::RedefineCommand::undo()
{
	m_item->impl->m_mouseEventMode = Impl::meNormal;
	m_item->updateMouseCursor();

	m_item->impl->m_setting.defined = true;
	m_item->updateActorSettings();
}
