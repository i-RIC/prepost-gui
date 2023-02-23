#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRIDATTRIBUTEDISPLAYSETTINGEDITDIALOG_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRIDATTRIBUTEDISPLAYSETTINGEDITDIALOG_H

#include "../preprocessorgridcrosssectionwindow2.h"

#include <QDialog>

class ColorMapSettingEditWidgetI;

namespace Ui {
class PreProcessorGridCrosssectionWindow2_GridAttributeDisplaySettingEditDialog;
}

class PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog : public QDialog
{
	Q_OBJECT

public:
	GridAttributeDisplaySettingEditDialog(GridAttributeDisplaySettingContainer* setting, PreProcessorGridCrosssectionWindow2* window);
	~GridAttributeDisplaySettingEditDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	GridAttributeDisplaySettingContainer setting() const;
	void setSetting(const GridAttributeDisplaySettingContainer& setting);
	void apply();

	bool m_applied;
	ColorMapSettingEditWidgetI* m_colorMapEditWidget;
	GridAttributeDisplaySettingContainer* m_setting;
	PreProcessorGridCrosssectionWindow2* m_window;

	Ui::PreProcessorGridCrosssectionWindow2_GridAttributeDisplaySettingEditDialog *ui;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_GRIDATTRIBUTEDISPLAYSETTINGEDITDIALOG_H
