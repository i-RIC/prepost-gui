#ifndef POST3DWINDOWCELLCONTOURGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWCELLCONTOURGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post3dwindowcellcontourgroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post3dWindowCellContourGroupDataItem_SettingEditWidget;
}

class ColorMapSettingEditWidgetWithImportExportButton;

class Post3dWindowCellContourGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post3dWindowCellContourGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

public slots:
	void setAsDefault();

private:
	ColorMapSettingEditWidgetWithImportExportButton* m_colorMapWidget;
	Post3dWindowCellContourGroupDataItem* m_item;

	Ui::Post3dWindowCellContourGroupDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWCELLCONTOURGROUPDATAITEM_SETTINGEDITWIDGET_H
