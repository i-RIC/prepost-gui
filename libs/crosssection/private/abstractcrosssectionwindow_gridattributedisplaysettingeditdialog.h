#ifndef ABSTRACTCROSSSECTIONWINDOW_GRIDATTRIBUTEDISPLAYSETTINGEDITDIALOG_H
#define ABSTRACTCROSSSECTIONWINDOW_GRIDATTRIBUTEDISPLAYSETTINGEDITDIALOG_H

#include "../abstractcrosssectionwindow.h"

#include <QDialog>

class ColorMapSettingEditWidgetI;

namespace Ui {
class AbstractCrosssectionWindow_GridAttributeDisplaySettingEditDialog;
}

class AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog : public QDialog
{
	Q_OBJECT

public:
	GridAttributeDisplaySettingEditDialog(GridAttributeDisplaySettingContainer* setting, AbstractCrosssectionWindow* window);
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
	AbstractCrosssectionWindow* m_window;

	Ui::AbstractCrosssectionWindow_GridAttributeDisplaySettingEditDialog *ui;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_GRIDATTRIBUTEDISPLAYSETTINGEDITDIALOG_H
