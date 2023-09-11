#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindownodescalargroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dWindowNodeScalarGroupDataItem_SettingEditWidget;
}

class Post2dWindowNodeScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowNodeScalarGroupDataItem* item, QWidget *parent = nullptr);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply);

private:
	ModifyCommandWidget* m_colorMapWidget;
	Post2dWindowNodeScalarGroupDataItem* m_item;

	Ui::Post2dWindowNodeScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
