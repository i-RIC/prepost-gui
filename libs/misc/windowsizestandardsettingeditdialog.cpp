#include "windowsizestandardsetting.h"
#include "windowsizestandardsettingeditdialog.h"
#include "ui_windowsizestandardsettingeditdialog.h"

WindowSizeStandardSettingEditDialog::WindowSizeStandardSettingEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::WindowSizeStandardSettingEditDialog)
{
	ui->setupUi(this);
}

WindowSizeStandardSettingEditDialog::~WindowSizeStandardSettingEditDialog()
{
	delete ui;
}

WindowSizeStandardSetting WindowSizeStandardSettingEditDialog::setting() const
{
	WindowSizeStandardSetting ret;
	ret.width = ui->widthSpinBox->value();
	ret.height = ui->heightSpinBox->value();
	ret.comment = ui->commentEdit->text();

	return ret;
}

void WindowSizeStandardSettingEditDialog::setSetting(const WindowSizeStandardSetting& setting)
{
	ui->widthSpinBox->setValue(setting.width);
	ui->heightSpinBox->setValue(setting.height);
	ui->commentEdit->setText(setting.comment);
}
