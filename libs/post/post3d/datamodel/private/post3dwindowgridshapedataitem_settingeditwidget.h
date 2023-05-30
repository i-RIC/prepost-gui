#ifndef POST3DWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H

#include "../post3dwindowgridshapedataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post3dWindowGridShapeDataItem_SettingEditWidget;
}

class Post3dWindowGridShapeDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post3dWindowGridShapeDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	Post3dWindowGridShapeDataItem* m_item;
	Ui::Post3dWindowGridShapeDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H
