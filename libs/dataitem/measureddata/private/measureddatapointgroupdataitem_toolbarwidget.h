#ifndef MEASUREDDATAPOINTGROUPDATAITEM_TOOLBARWIDGET_H
#define MEASUREDDATAPOINTGROUPDATAITEM_TOOLBARWIDGET_H

#include "../measureddatapointgroupdataitem.h"

#include <QWidget>

namespace Ui {
class MeasuredDataPointGroupDataItem_ToolBarWidget;
}

class MeasuredDataPointGroupDataItem::ToolBarWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ToolBarWidget(const Setting& setting, MeasuredDataPointGroupDataItem* item, QWidget *parent);
	~ToolBarWidget();

private slots:
	void applySetting();
	void updateActorSetting();

private:
	void myApplySetting(const Setting& setting);
	Setting modifiedSetting() const;

	MeasuredDataPointGroupDataItem* m_item;
	Ui::MeasuredDataPointGroupDataItem_ToolBarWidget *ui;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_TOOLBARWIDGET_H
