#include "gridcreatingconditionlaplacedeploysettingdialog.h"
#include "ui_gridcreatingconditionlaplacedeploysettingdialog.h"

GridCreatingConditionLaplaceDeploySettingDialog::GridCreatingConditionLaplaceDeploySettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionLaplaceDeploySettingDialog)
{
	ui->setupUi(this);
}

GridCreatingConditionLaplaceDeploySettingDialog::~GridCreatingConditionLaplaceDeploySettingDialog()
{
	delete ui;
}

GridCreatingConditionLaplace::DivisionMode GridCreatingConditionLaplaceDeploySettingDialog::divisionMode()
{
	if (ui->equalRadioButton->isChecked()) {
		return GridCreatingConditionLaplace::DivisionMode::Equally;
	} else {
		return GridCreatingConditionLaplace::DivisionMode::Geometric;
	}
}

void GridCreatingConditionLaplaceDeploySettingDialog::setDivisionMode(GridCreatingConditionLaplace::DivisionMode mode)
{
	if (mode == GridCreatingConditionLaplace::DivisionMode::Auto) {
		ui->equalRadioButton->setChecked(true);
	}	else if (mode == GridCreatingConditionLaplace::DivisionMode::Equally) {
		ui->equalRadioButton->setChecked(true);
	} else {
		ui->geometricRadioButton->setChecked(true);
	}
}

double GridCreatingConditionLaplaceDeploySettingDialog::commonRatio()
{
	return ui->commonRatioSpinBox->value();
}

void GridCreatingConditionLaplaceDeploySettingDialog::setCommonRatio(double ratio)
{
	ui->commonRatioSpinBox->setValue(ratio);
}
