#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindowpolydatagroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dWindowPolyDataGroupDataItem_SettingEditWidget;
}

class Post2dWindowPolyDataGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowPolyDataGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	Post2dWindowPolyDataGroupDataItem* m_item;
	Ui::Post2dWindowPolyDataGroupDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_SETTINGEDITWIDGET_H
