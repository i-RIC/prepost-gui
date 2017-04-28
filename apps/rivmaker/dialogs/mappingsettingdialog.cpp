#include "mappingsettingdialog.h"
#include "ui_mappingsettingdialog.h"

MappingSettingDialog::MappingSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MappingSettingDialog)
{
	ui->setupUi(this);
}

MappingSettingDialog::~MappingSettingDialog()
{
	delete ui;
}
