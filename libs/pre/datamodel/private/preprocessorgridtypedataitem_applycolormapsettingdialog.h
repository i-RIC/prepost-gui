#ifndef PREPROCESSORGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGDIALOG_H
#define PREPROCESSORGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGDIALOG_H

#include "../preprocessorgridtypedataitem.h"

#include <misc/modifycommanddialog.h>

class PreProcessorGridTypeDataItem::ApplyColorMapSettingDialog : public ModifyCommandDialog
{
public:
	ApplyColorMapSettingDialog(const std::string& name, QWidget* parent, PreProcessorGridTypeDataItem* item);

	void pushCommand(QUndoCommand* command) override;

private:
	std::string m_name;
	PreProcessorGridTypeDataItem* m_item;
};

#endif // PREPROCESSORGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGDIALOG_H
