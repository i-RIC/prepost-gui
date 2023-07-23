#include "addconnectiondialog.h"
#include "ui_addconnectiondialog.h"

AddConnectionDialog::AddConnectionDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AddConnectionDialog)
{
	ui->setupUi(this);
}

AddConnectionDialog::~AddConnectionDialog()
{
	delete ui;
}
