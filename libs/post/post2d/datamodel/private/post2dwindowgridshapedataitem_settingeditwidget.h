#ifndef POST2DWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindowgridshapedataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dWindowGridShapeDataItem_SettingEditWidget;
}

class Post2dWindowGridShapeDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowGridShapeDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	Post2dWindowGridShapeDataItem* m_item;
	Ui::Post2dWindowGridShapeDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H
