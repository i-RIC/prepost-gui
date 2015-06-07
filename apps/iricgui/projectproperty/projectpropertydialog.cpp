#include "ui_projectpropertydialog.h"

#include "projectpropertydialog.h"

ProjectPropertyDialog::ProjectPropertyDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::ProjectPropertyDialog)
{
	ui->setupUi(this);
}

ProjectPropertyDialog::~ProjectPropertyDialog()
{
	delete ui;
}

void ProjectPropertyDialog::setProjectData(ProjectData* data)
{
	ui->basicInfoTab->setProjectData(data);
}
