#ifndef PREPROCESSORGRIDATTRIBUTEABSTRACTCELLDATAITEM_PROPERTYDIALOG_H
#define PREPROCESSORGRIDATTRIBUTEABSTRACTCELLDATAITEM_PROPERTYDIALOG_H

#include "../preprocessorgridattributeabstractcelldataitem.h"

#include <QDialog>

class ColorMapSettingEditWidgetI;
class GridAttributeCellSetting;
class PreProcessorGridAttributeAbstractCellGroupDataItem;

namespace Ui {
class PreProcessorGridAttributeAbstractCellDataItem_PropertyDialog;
}

class PreProcessorGridAttributeAbstractCellDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(PreProcessorGridAttributeAbstractCellGroupDataItem* item, QWidget *parent);

	~PropertyDialog();

	void hideLineWidth();

	ColorMapSettingEditWidgetI* widget() const;
	void setWidget(ColorMapSettingEditWidgetI* w);

	void setSetting(GridAttributeCellSetting* setting);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();
	QUndoCommand* createModifyCommand(bool apply);

	bool m_applied;
	PreProcessorGridAttributeAbstractCellGroupDataItem* m_item;

	Ui::PreProcessorGridAttributeAbstractCellDataItem_PropertyDialog *ui;
};

#endif // PREPROCESSORGRIDATTRIBUTEABSTRACTCELLDATAITEM_PROPERTYDIALOG_H
