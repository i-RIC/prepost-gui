#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_DISPLAYSETTINGDIALOG_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_DISPLAYSETTINGDIALOG_H

#include "preprocessorgridcrosssectionwindow2_displaysettingcontainer.h"
#include "../preprocessorgridcrosssectionwindow2.h"

#include <QDialog>

namespace Ui {
class PreProcessorGridCrosssectionWindow2_DisplaySettingDialog;
}

class PreProcessorGridCrosssectionWindow2::DisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DisplaySettingDialog(PreProcessorGridCrosssectionWindow2* window);
	~DisplaySettingDialog();

	QUndoCommand* createModifyCommand(bool apply);

	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	DisplaySettingContainer setting() const;
	void setSetting(const DisplaySettingContainer& setting);

	bool m_applied;

	DisplaySettingContainer m_setting;
	PreProcessorGridCrosssectionWindow2* m_window;

	Ui::PreProcessorGridCrosssectionWindow2_DisplaySettingDialog *ui;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_DISPLAYSETTINGDIALOG_H
