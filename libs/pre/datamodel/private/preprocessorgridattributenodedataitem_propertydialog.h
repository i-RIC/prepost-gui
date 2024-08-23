#ifndef PREPROCESSORGRIDATTRIBUTENODEDATAITEM_PROPERTYDIALOG_H
#define PREPROCESSORGRIDATTRIBUTENODEDATAITEM_PROPERTYDIALOG_H

#include "../preprocessorgridattributenodedataitem.h"

#include <QDialog>

class ColorMapSettingEditWidgetI;
class GridAttributeNodeSetting;
class PreProcessorGridAttributeNodeGroupDataItem;

namespace Ui {
class PreProcessorGridAttributeNodeDataItem_PropertyDialog;
}


class PreProcessorGridAttributeNodeDataItem::PropertyDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyDialog(PreProcessorGridAttributeNodeGroupDataItem* item, QWidget *parent);
	~PropertyDialog();

	void hideLineWidth();

	ColorMapSettingEditWidgetI* widget() const;
	void setWidget(ColorMapSettingEditWidgetI* w);

	void setSetting(GridAttributeNodeSetting* setting);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();
	QUndoCommand* createModifyCommand(bool apply);

	bool m_applied;
	PreProcessorGridAttributeNodeGroupDataItem* m_item;

	Ui::PreProcessorGridAttributeNodeDataItem_PropertyDialog *ui;
};

#endif // PREPROCESSORGRIDATTRIBUTENODEDATAITEM_PROPERTYDIALOG_H
