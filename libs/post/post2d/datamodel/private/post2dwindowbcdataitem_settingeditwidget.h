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
	explicit SettingEditWidget(QWidget *parent = nullptr);
	~SettingEditWidget();

private:
	Ui::Post2dWindowBCDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWBCDATAITEM_SETTINGEDITWIDGET_H
