#ifndef PREPROCESSORGRIDATTRIBUTECELLDATAITEM_PROPERTYDIALOG_H
#define PREPROCESSORGRIDATTRIBUTECELLDATAITEM_PROPERTYDIALOG_H

#include "../preprocessorgridattributecelldataitem.h"

#include <QDialog>

class ColorMapSettingEditWidgetI;
class PreProcessorGridAttributeCellGroupDataItem;

namespace Ui {
class PreProcessorGridAttributeCellDataItem_PropertyDialog;
}

class PreProcessorGridAttributeCellDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PropertyDialog::PropertyDialog(PreProcessorGridAttributeCellGroupDataItem* item, QWidget *parent = nullptr);

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
	PreProcessorGridAttributeCellGroupDataItem* m_item;

	Ui::PreProcessorGridAttributeCellDataItem_PropertyDialog *ui;
};

#endif // PREPROCESSORGRIDATTRIBUTECELLDATAITEM_PROPERTYDIALOG_H
