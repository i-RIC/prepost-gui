#ifndef ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGDIALOG_H
#define ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGDIALOG_H

#include "abstractcrosssectionwindow_displaysettingcontainer.h"
#include "../abstractcrosssectionwindow.h"

#include <QDialog>

namespace Ui {
class AbstractCrosssectionWindow_DisplaySettingDialog;
}

class AbstractCrosssectionWindow::DisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit DisplaySettingDialog(AbstractCrosssectionWindow* window);
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
	AbstractCrosssectionWindow* m_window;

	Ui::AbstractCrosssectionWindow_DisplaySettingDialog *ui;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_DISPLAYSETTINGDIALOG_H
