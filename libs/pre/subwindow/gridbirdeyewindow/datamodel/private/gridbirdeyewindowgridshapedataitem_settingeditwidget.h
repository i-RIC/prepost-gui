#ifndef GRIDBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H
#define GRIDBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H

#include "../gridbirdeyewindowgridshapedataitem.h"

#include <misc/modifycommandwidget.h>

#include <string>
#include <vector>

namespace Ui {
class GridBirdEyeWindowGridShapeDataItem_SettingEditWidget;
}

class GridBirdEyeWindowGridShapeDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(GridBirdEyeWindowGridShapeDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

	Setting setting() const;
	void setSetting(const Setting& setting);
	void setSetting(Setting* setting);

private:
	Setting* m_setting;
	std::vector<std::string> m_valueNames;
	GridBirdEyeWindowGridShapeDataItem* m_item;

	Ui::GridBirdEyeWindowGridShapeDataItem_SettingEditWidget *ui;
};

#endif // GRIDBIRDEYEWINDOWGRIDSHAPEDATAITEM_SETTINGEDITWIDGET_H
