#include "projecttimesettingdialog.h"
#include "ui_projecttimesettingdialog.h"

ProjectTimeSettingDialog::ProjectTimeSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ProjectTimeSettingDialog)
{
	ui->setupUi(this);
}

ProjectTimeSettingDialog::~ProjectTimeSettingDialog()
{
	delete ui;
}
