#ifndef PREPROCESSORGRIDATTRIBUTECELLDATAITEM_PROPERTYDIALOG_H
#define PREPROCESSORGRIDATTRIBUTECELLDATAITEM_PROPERTYDIALOG_H

#include "../preprocessorgridattributecelldataitem.h"

#include <QDialog>

class ColorMapSettingEditWidgetI;
class OpacityContainer;
class PreProcessorGridAttributeCellGroupDataItem;

namespace Ui {
class PreProcessorGridAttributeCellDataItem_PropertyDialog;
}

class PreProcessorGridAttributeCellDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(PreProcessorGridAttributeCellGroupDataItem* item, QWidget *parent);

	~PropertyDialog();

	ColorMapSettingEditWidgetI* widget() const;
	void setWidget(ColorMapSettingEditWidgetI* w);

	OpacityContainer opacity() const;
	void setOpacity(const OpacityContainer& opacity);

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
