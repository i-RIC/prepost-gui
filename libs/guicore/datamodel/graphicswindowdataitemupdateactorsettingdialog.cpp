#include "graphicswindowdataitem.h"
#include "graphicswindowdataitemupdateactorsettingdialog.h"

GraphicsWindowDataItemUpdateActorSettingDialog::GraphicsWindowDataItemUpdateActorSettingDialog(GraphicsWindowDataItem* item, QWidget* parent) :
	ModifyCommandDialog(parent),
	m_item {item}
{}

void GraphicsWindowDataItemUpdateActorSettingDialog::pushCommand(QUndoCommand* command)
{
	m_item->pushUpdateActorSettingCommand(command, m_item, true);
}
