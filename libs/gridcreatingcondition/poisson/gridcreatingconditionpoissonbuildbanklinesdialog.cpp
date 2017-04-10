#include "gridcreatingconditionpoissonbuildbanklinesdialog.h"
#include "ui_gridcreatingconditionpoissonbuildbanklinesdialog.h"

GridCreatingConditionPoissonBuildBankLinesDialog::GridCreatingConditionPoissonBuildBankLinesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionPoissonBuildBankLinesDialog)
{
	ui->setupUi(this);
}

GridCreatingConditionPoissonBuildBankLinesDialog::~GridCreatingConditionPoissonBuildBankLinesDialog()
{
	delete ui;
}

double GridCreatingConditionPoissonBuildBankLinesDialog::leftBankDistance() const
{
	return ui->leftBankSpinBox->value();
}

void GridCreatingConditionPoissonBuildBankLinesDialog::setLeftBankDistance(double dist)
{
	ui->leftBankSpinBox->setValue(dist);
}

double GridCreatingConditionPoissonBuildBankLinesDialog::rightBankDistance() const
{
	return ui->rightBankSpinBox->value();
}

void GridCreatingConditionPoissonBuildBankLinesDialog::setRightBankDistance(double dist)
{
	ui->rightBankSpinBox->setValue(dist);
}
