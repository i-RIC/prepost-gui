#include "preprocessorgridtypedataitem_applycolormapsettingandrendercommand.h"
#include "preprocessorgridtypedataitem_applycolormapsettingdialog.h"

PreProcessorGridTypeDataItem::ApplyColorMapSettingDialog::ApplyColorMapSettingDialog(const std::string& name, QWidget* parent, PreProcessorGridTypeDataItem* item) :
	ModifyCommandDialog {parent},
	m_name {name},
	m_item {item}
{}

void PreProcessorGridTypeDataItem::ApplyColorMapSettingDialog::pushCommand(QUndoCommand* command)
{
	m_item->pushCommand(new ApplyColorMapSettingAndRenderCommand(command, m_name, true, m_item));
}
