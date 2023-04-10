#ifndef POST2DWINDOWGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGDIALOG_H
#define POST2DWINDOWGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGDIALOG_H

#include "../post2dwindowgridtypedataitem.h"

#include <misc/modifycommanddialog.h>

class Post2dWindowGridTypeDataItem::ApplyColorMapSettingDialog : public ModifyCommandDialog
{
public:
	ApplyColorMapSettingDialog(const std::string& name, QWidget* parent, Post2dWindowGridTypeDataItem* item);

	void pushCommand(QUndoCommand* command) override;

private:
	std::string m_name;
	Post2dWindowGridTypeDataItem* m_item;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_APPLYCOLORMAPSETTINGDIALOG_H
