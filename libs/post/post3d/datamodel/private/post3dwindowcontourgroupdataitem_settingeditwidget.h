#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post3dwindowcontourgroupdataitem.h"

#include <misc/modifycommandwidget.h>

#include <QWidget>

namespace Ui {
class Post3dWindowContourGroupDataItem_SettingEditWidget;
}

class ColorMapSettingEditWidget;

class Post3dWindowContourGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post3dWindowContourGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	Setting setting() const;
	void setSetting(const Setting& setting);

	ModifyCommandWidget* m_colorMapWidget;
	Post3dWindowContourGroupDataItem* m_item;

	Ui::Post3dWindowContourGroupDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_SETTINGEDITWIDGET_H
