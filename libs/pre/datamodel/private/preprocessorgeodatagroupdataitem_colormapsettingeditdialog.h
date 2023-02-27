#ifndef PREPROCESSORGEODATAGROUPDATAITEM_COLORMAPSETTINGEDITDIALOG_H
#define PREPROCESSORGEODATAGROUPDATAITEM_COLORMAPSETTINGEDITDIALOG_H

#include "../preprocessorgeodatagroupdataitem.h"

#include <QDialog>

namespace Ui {
class PreProcessorGeoDataGroupDataItem_ColorMapSettingEditDialog;
}

class PreProcessorGridTypeDataItem;

class PreProcessorGeoDataGroupDataItem::ColorMapSettingEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ColorMapSettingEditDialog(PreProcessorGeoDataGroupDataItem* item, QWidget *parent = nullptr);
	~ColorMapSettingEditDialog();

	ColorMapSettingEditWidgetI* widget();
	void setWidget(ColorMapSettingEditWidgetI* w);

	ColorMapSettingContainerI* setting() const;
	void setSetting(ColorMapSettingContainerI* setting);

	QUndoCommand* createModifyCommand() const;

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	bool m_applied;
	PreProcessorGridTypeDataItem* m_gridTypeDataItem;
	PreProcessorGeoDataGroupDataItem* m_item;

	ColorMapSettingEditWidgetI* m_widget;
	Ui::PreProcessorGeoDataGroupDataItem_ColorMapSettingEditDialog *ui;
};

#endif // PREPROCESSORGEODATAGROUPDATAITEM_COLORMAPSETTINGEDITDIALOG_H
