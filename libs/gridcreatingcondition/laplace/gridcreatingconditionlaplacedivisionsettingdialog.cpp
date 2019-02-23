#include "gridcreatingconditionlaplacedivisionsettingdialog.h"
#include "ui_gridcreatingconditionlaplacedivisionsettingdialog.h"

GridCreatingConditionLaplaceDivisionSettingDialog::GridCreatingConditionLaplaceDivisionSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionLaplaceDivisionSettingDialog)
{
	ui->setupUi(this);
}

GridCreatingConditionLaplaceDivisionSettingDialog::~GridCreatingConditionLaplaceDivisionSettingDialog()
{
	delete ui;
}

int GridCreatingConditionLaplaceDivisionSettingDialog::divisionNumber() const
{
	return ui->spinBox->value();
}

void GridCreatingConditionLaplaceDivisionSettingDialog::setDivisionNumber(int num)
{
	ui->spinBox->setValue(num);
}
