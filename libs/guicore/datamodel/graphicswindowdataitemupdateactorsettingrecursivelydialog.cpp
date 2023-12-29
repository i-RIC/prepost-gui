#include "graphicswindowdataitem.h"
#include "graphicswindowdataitemupdateactorsettingrecursivelydialog.h"

GraphicsWindowDataItemUpdateActorSettingRecursivelyDialog::GraphicsWindowDataItemUpdateActorSettingRecursivelyDialog(GraphicsWindowDataItem* item, QWidget* parent) :
	ModifyCommandDialog(parent),
	m_item {item}
{}

void GraphicsWindowDataItemUpdateActorSettingRecursivelyDialog::pushCommand(QUndoCommand* command)
{
        m_item->pushUpdateActorSettingRecursivelyCommand(command, m_item, true);
}
