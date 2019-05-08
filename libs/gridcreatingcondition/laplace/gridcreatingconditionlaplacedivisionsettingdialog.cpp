#include "gridcreatingconditionlaplace.h"
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
	return ui->divisionNumberSpinBox->value();
}

void GridCreatingConditionLaplaceDivisionSettingDialog::setDivisionNumber(int num)
{
	ui->divisionNumberSpinBox->setValue(num);
}

GridCreatingConditionLaplace::DivisionMode GridCreatingConditionLaplaceDivisionSettingDialog::divisionMode() const
{
	if (ui->autoRadioButton->isChecked()) {
		return GridCreatingConditionLaplace::DivisionMode::Auto;
	} else if (ui->equalRadioButton->isChecked()) {
		return GridCreatingConditionLaplace::DivisionMode::Equally;
	} else if (ui->geometricRadioButton->isChecked()) {
		return GridCreatingConditionLaplace::DivisionMode::Geometric;
	}
}

void GridCreatingConditionLaplaceDivisionSettingDialog::setDivisionMode(GridCreatingConditionLaplace::DivisionMode mode)
{
	if (mode == GridCreatingConditionLaplace::DivisionMode::Auto) {
		ui->autoRadioButton->setChecked(true);
	} else if (mode == GridCreatingConditionLaplace::DivisionMode::Equally) {
		ui->equalRadioButton->setChecked(true);
	} else {
		ui->geometricRadioButton->setChecked(true);
	}
}

double GridCreatingConditionLaplaceDivisionSettingDialog::commonRatio() const
{
	return ui->commonRatioSpinBox->value();
}

void GridCreatingConditionLaplaceDivisionSettingDialog::setCommonRatio(double ratio)
{
	ui->commonRatioSpinBox->setValue(ratio);
}

bool GridCreatingConditionLaplaceDivisionSettingDialog::thisLineOnly() const
{
	return ui->thisOnlyRadioButton->isChecked();
}
