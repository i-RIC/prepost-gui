#ifndef POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindowabstractcellscalargroupdataitem.h"

#include <misc/modifycommandwidget.h>
#include <QWidget>

namespace Ui {
class Post2dWindowAbstractCellScalarGroupDataItem_SettingEditWidget;
}

class ModifyCommandWidget;

class Post2dWindowAbstractCellScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowAbstractCellScalarGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	void hideLineWidth();
	QUndoCommand* createModifyCommand(bool apply);

private:
	ModifyCommandWidget* m_colorMapWidget;
	Post2dWindowAbstractCellScalarGroupDataItem* m_item;

	Ui::Post2dWindowAbstractCellScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
