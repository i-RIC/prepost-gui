#include "tpoexporter_settingdialog.h"
#include "ui_tpoexporter_settingdialog.h"

TpoExporter::SettingDialog::SettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::TpoExporter_SettingDialog)
{
	ui->setupUi(this);
}

TpoExporter::SettingDialog::~SettingDialog()
{
	delete ui;
}

int TpoExporter::SettingDialog::xPrecision() const
{
	return ui->xSpinBox->value();
}

int TpoExporter::SettingDialog::yPrecision() const
{
	return ui->ySpinBox->value();
}

int TpoExporter::SettingDialog::zPrecision() const
{
	return ui->zSpinBox->value();
}
