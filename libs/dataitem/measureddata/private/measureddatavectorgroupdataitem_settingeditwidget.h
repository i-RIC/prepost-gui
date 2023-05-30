#ifndef MEASUREDDATAVECTORGROUPDATAITEM_SETTINGEDITWIDGET_H
#define MEASUREDDATAVECTORGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../measureddatavectorgroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class MeasuredDataVectorGroupDataItem_SettingEditWidget;
}

class GeneralFilteringSettingEditWidget;

class MeasuredDataVectorGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(MeasuredDataVectorGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	GeneralFilteringSettingEditWidget* m_filteringEditWidget;

	MeasuredDataVectorGroupDataItem* m_item;
	Ui::MeasuredDataVectorGroupDataItem_SettingEditWidget *ui;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_SETTINGEDITWIDGET_H
