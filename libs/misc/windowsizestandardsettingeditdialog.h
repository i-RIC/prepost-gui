#ifndef WINDOWSIZESTANDARDSETTINGEDITDIALOG_H
#define WINDOWSIZESTANDARDSETTINGEDITDIALOG_H

#include "misc_global.h"

#include <QDialog>

namespace Ui {
class WindowSizeStandardSettingEditDialog;
}

class WindowSizeStandardSetting;

class MISCDLL_EXPORT WindowSizeStandardSettingEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit WindowSizeStandardSettingEditDialog(QWidget *parent = nullptr);
	~WindowSizeStandardSettingEditDialog();

	WindowSizeStandardSetting setting() const;
	void setSetting(const WindowSizeStandardSetting& setting);

private:
	Ui::WindowSizeStandardSettingEditDialog *ui;
};

#endif // WINDOWSIZESTANDARDSETTINGEDITDIALOG_H
