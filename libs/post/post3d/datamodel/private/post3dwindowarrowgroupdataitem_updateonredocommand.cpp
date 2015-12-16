#include "post3dwindowarrowgroupdataitem_updateonredocommand.h"

Post3dWindowArrowGroupDataItem::UpdateOnRedoCommand::UpdateOnRedoCommand(Post3dWindowArrowGroupDataItem* item) :
	QUndoCommand(),
	m_item {item}
{}

void Post3dWindowArrowGroupDataItem::UpdateOnRedoCommand::redo()
{
	m_item->updateActorSettings();
	m_item->renderGraphicsView();
}

void Post3dWindowArrowGroupDataItem::UpdateOnRedoCommand::undo()
{}
