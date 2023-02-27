#ifndef PREPROCESSORGRIDATTRIBUTENODEDATAITEM_PROPERTYDIALOG_H
#define PREPROCESSORGRIDATTRIBUTENODEDATAITEM_PROPERTYDIALOG_H

#include "../preprocessorgridattributenodedataitem.h"

#include <QDialog>

class ColorMapSettingEditWidgetI;
class PreProcessorGridAttributeNodeGroupDataItem;

namespace Ui {
class PreProcessorGridAttributeNodeDataItem_PropertyDialog;
}

class PreProcessorGridAttributeNodeDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog(PreProcessorGridAttributeNodeGroupDataItem* item, QWidget *parent = nullptr);
	~PropertyDialog();

	ColorMapSettingEditWidgetI* widget() const;
	void setWidget(ColorMapSettingEditWidgetI* w);

	int opacityPercent() const;
	void setOpacityPercent(int opacity);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;
	PreProcessorGridAttributeNodeGroupDataItem* m_item;

	Ui::PreProcessorGridAttributeNodeDataItem_PropertyDialog *ui;
};

#endif // PREPROCESSORGRIDATTRIBUTENODEDATAITEM_PROPERTYDIALOG_H
