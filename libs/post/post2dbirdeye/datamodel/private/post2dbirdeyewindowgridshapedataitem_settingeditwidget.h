#ifndef POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H
#define POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dbirdeyewindowgridshapedataitem.h"

#include <misc/modifycommandwidget.h>

#include <string>
#include <vector>

namespace Ui {
class Post2dBirdEyeWindowGridShapeDataItem_SettingEditWidget;
}

class Post2dBirdEyeWindowGridShapeDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dBirdEyeWindowGridShapeDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

	Setting setting() const;
	void setSetting(const Setting& setting);
	void setSetting(Setting* setting);

private:
	Setting* m_setting;
	std::vector<std::string> m_valueNames;
	Post2dBirdEyeWindowGridShapeDataItem* m_item;

	Ui::Post2dBirdEyeWindowGridShapeDataItem_SettingEditWidget *ui;
};

#endif // POST2DBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H
