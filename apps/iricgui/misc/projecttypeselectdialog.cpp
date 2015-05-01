#include "ui_projecttypeselectdialog.h"

#include "projecttypeselectdialog.h"

ProjectTypeSelectDialog::ProjectTypeSelectDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::ProjectTypeSelectDialog)
{
	ui->setupUi(this);
}

ProjectTypeSelectDialog::~ProjectTypeSelectDialog()
{
	delete ui;
}

bool ProjectTypeSelectDialog::folderProject()
{
	return ui->projectRadioButton->isChecked();
}
