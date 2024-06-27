#ifndef POST3DWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post3dwindownodescalargroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post3dWindowNodeScalarGroupDataItem_SettingEditWidget;
}

class Post3dWindowGridTypeDataItem;
class v4PostZoneDataContainer;

class Post3dWindowNodeScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post3dWindowNodeScalarGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	void setGridTypeDataItem(Post3dWindowGridTypeDataItem* item);
	void setZoneData(v4PostZoneDataContainer* zoneData);

	Setting setting() const;
	void setSetting(const Setting& setting);

	QUndoCommand* createModifyCommand(bool apply) override;

private slots:
	void targetChanged(int index);
	void fullRangeChanged(bool full);
	void iMinChanged(int min);
	void iMaxChanged(int max);
	void jMinChanged(int min);
	void jMaxChanged(int max);
	void kMinChanged(int min);
	void kMaxChanged(int max);

private:
	Post3dWindowNodeScalarGroupDataItem* m_item;
	std::vector<std::string> m_targets;
	Post3dWindowGridTypeDataItem* m_gridTypeDataItem;

	Ui::Post3dWindowNodeScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
