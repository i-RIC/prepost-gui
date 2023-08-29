#include "modeldeletedialog.h"
#include "ui_modeldeletedialog.h"

#include <QDir>

ModelDeleteDialog::ModelDeleteDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ModelDeleteDialog)
{
	ui->setupUi(this);
}

ModelDeleteDialog::~ModelDeleteDialog()
{
	delete ui;
}

void ModelDeleteDialog::setFolder(const QString& folder)
{
	ui->folderValueLabel->setText(QDir::toNativeSeparators(folder));
}

ModelDeleteDialog::DeleteMode ModelDeleteDialog::deleteMode() const
{
	if (ui->keepRadioButton->isChecked()) {
		return DeleteMode::Keep;
	} else if (ui->deleteRadioButton->isChecked()) {
		return DeleteMode::Delete;
	}
}
