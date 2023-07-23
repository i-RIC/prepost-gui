#include "addmodeldialog.h"
#include "ui_addmodeldialog.h"

AddModelDialog::AddModelDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AddModelDialog)
{
	ui->setupUi(this);
	QList<int> sizes;
	sizes << 100 << 400;
	ui->splitter->setSizes(sizes);
}

AddModelDialog::~AddModelDialog()
{
	delete ui;
}
