#include "post2dwindowgridtypedataitem_applycolormapsettingdialog.h"
#include "post2dwindowgridtypedataitem_applycolormapsettingcommand.h"

Post2dWindowGridTypeDataItem::ApplyColorMapSettingDialog::ApplyColorMapSettingDialog(const std::string& name, QWidget* parent, Post2dWindowGridTypeDataItem* item) :
	ModifyCommandDialog {parent},
	m_name {name},
	m_item {item}
{}

void Post2dWindowGridTypeDataItem::ApplyColorMapSettingDialog::pushCommand(QUndoCommand* command)
{
	m_item->pushRenderCommand(new ApplyColorMapSettingCommand(command, m_name, true, m_item), m_item);
}
