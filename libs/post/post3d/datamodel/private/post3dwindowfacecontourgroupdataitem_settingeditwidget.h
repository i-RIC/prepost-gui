#ifndef POST3DWINDOWFACECONTOURGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWFACECONTOURGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post3dwindowfacecontourgroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post3dWindowFaceContourGroupDataItem_SettingEditWidget;
}

class ColorMapSettingEditWidgetWithImportExportButton;

class Post3dWindowFaceContourGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post3dWindowFaceContourGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

public slots:
	void setAsDefault();

private:
	ColorMapSettingEditWidgetWithImportExportButton* m_colorMapWidget;
	Post3dWindowFaceContourGroupDataItem* m_item;

	Ui::Post3dWindowFaceContourGroupDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWFACECONTOURGROUPDATAITEM_SETTINGEDITWIDGET_H
