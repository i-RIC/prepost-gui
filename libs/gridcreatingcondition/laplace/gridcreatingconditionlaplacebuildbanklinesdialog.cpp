#include "gridcreatingconditionlaplacebuildbanklinesdialog.h"
#include "ui_gridcreatingconditionlaplacebuildbanklinesdialog.h"

GridCreatingConditionLaplaceBuildBankLinesDialog::GridCreatingConditionLaplaceBuildBankLinesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionLaplaceBuildBankLinesDialog)
{
	ui->setupUi(this);
}

GridCreatingConditionLaplaceBuildBankLinesDialog::~GridCreatingConditionLaplaceBuildBankLinesDialog()
{
	delete ui;
}

double GridCreatingConditionLaplaceBuildBankLinesDialog::leftBankDistance() const
{
	return ui->leftBankSpinBox->value();
}

void GridCreatingConditionLaplaceBuildBankLinesDialog::setLeftBankDistance(double dist)
{
	ui->leftBankSpinBox->setValue(dist);
}

double GridCreatingConditionLaplaceBuildBankLinesDialog::rightBankDistance() const
{
	return ui->rightBankSpinBox->value();
}

void GridCreatingConditionLaplaceBuildBankLinesDialog::setRightBankDistance(double dist)
{
	ui->rightBankSpinBox->setValue(dist);
}
