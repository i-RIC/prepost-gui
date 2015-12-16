#include "post3dwindowarrowgroupdataitem_updateonundocommand.h"

Post3dWindowArrowGroupDataItem::UpdateOnUndoCommand::UpdateOnUndoCommand(Post3dWindowArrowGroupDataItem* item) :
	QUndoCommand(),
	m_item {item}
{}

void Post3dWindowArrowGroupDataItem::UpdateOnUndoCommand::redo()
{}

void Post3dWindowArrowGroupDataItem::UpdateOnUndoCommand::undo()
{
	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}
