#ifndef MEASUREDDATAPOINTGROUPDATAITEM_SETTINGEDITWIDGET_H
#define MEASUREDDATAPOINTGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../measureddatapointgroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class MeasuredDataPointGroupDataItem_SettingEditWidget;
}

class ColorMapSettingEditWidget;

class MeasuredDataPointGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(MeasuredDataPointGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private slots:
	void targetChanged(int index);

private:
	Setting setting() const;
	void setSetting(const Setting& setting);

	std::vector<std::string> m_targets;
	ColorMapSettingEditWidget* m_colorMapWidget;

	MeasuredDataPointGroupDataItem* m_item;
	Ui::MeasuredDataPointGroupDataItem_SettingEditWidget *ui;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_SETTINGEDITWIDGET_H
