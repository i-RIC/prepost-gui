#include "preprocessorbcgroupsettingdialog.h"
#include "ui_preprocessorbcgroupsettingdialog.h"

PreProcessorBcGroupSettingDialog::PreProcessorBcGroupSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorBcGroupSettingDialog)
{
	ui->setupUi(this);

	ui->fontSettingWidget->hideColor();
}

PreProcessorBcGroupSettingDialog::~PreProcessorBcGroupSettingDialog()
{
	delete ui;
}

vtkTextPropertySettingContainer PreProcessorBcGroupSettingDialog::nameSetting() const
{
	return ui->fontSettingWidget->setting();
}
void PreProcessorBcGroupSettingDialog::setNameSetting(const vtkTextPropertySettingContainer& setting)
{
	ui->fontSettingWidget->setSetting(setting);
}
