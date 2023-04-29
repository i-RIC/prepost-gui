#ifndef POST2DWINDOWCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindowcellscalargroupdataitem.h"

#include <misc/modifycommandwidget.h>
#include <QWidget>

namespace Ui {
class Post2dWindowCellScalarGroupDataItem_SettingEditWidget;
}

class ModifyCommandWidget;

class Post2dWindowCellScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowCellScalarGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply);

private:
	ModifyCommandWidget* m_colorMapWidget;
	Post2dWindowCellScalarGroupDataItem* m_item;

	Ui::Post2dWindowCellScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
