#ifndef POST2DWINDOWBCDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWBCDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindowbcdataitem.h"

#include <misc/modifycommandwidget.h>
#include <QWidget>

namespace Ui {
class Post2dWindowBCDataItem_SettingEditWidget;
}

class Post2dWindowBCDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(Post2dWindowBCDataItem* item, QWidget *parent = nullptr);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;
	Setting setting();
	void setSetting(const Setting& setting);

private:
	Post2dWindowBCDataItem* m_item;

	Ui::Post2dWindowBCDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWBCDATAITEM_SETTINGEDITWIDGET_H
