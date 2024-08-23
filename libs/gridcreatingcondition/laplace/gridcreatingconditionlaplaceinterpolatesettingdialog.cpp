#include "gridcreatingconditionlaplaceinterpolatesettingdialog.h"
#include "ui_gridcreatingconditionlaplaceinterpolatesettingdialog.h"

GridCreatingConditionLaplaceInterpolateSettingDialog::GridCreatingConditionLaplaceInterpolateSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionLaplaceInterpolateSettingDialog)
{
	ui->setupUi(this);
}

GridCreatingConditionLaplaceInterpolateSettingDialog::~GridCreatingConditionLaplaceInterpolateSettingDialog()
{
	delete ui;
}

GridCreatingConditionLaplace::InterpolationType GridCreatingConditionLaplaceInterpolateSettingDialog::interpolationType() const
{
	if (ui->splineRadioButton->isChecked()) {
		return GridCreatingConditionLaplace::InterpolationType::Spline;
	} else {
		return GridCreatingConditionLaplace::InterpolationType::Linear;
	}
}

void GridCreatingConditionLaplaceInterpolateSettingDialog::setInterpolationType(GridCreatingConditionLaplace::InterpolationType type)
{
	if (type == GridCreatingConditionLaplace::InterpolationType::Spline) {
		ui->splineRadioButton->setChecked(true);
	} else if (type == GridCreatingConditionLaplace::InterpolationType::Linear) {
		ui->polylineRadioButton->setChecked(true);
	}
}

bool GridCreatingConditionLaplaceInterpolateSettingDialog::thisLineOnly() const
{
	return ui->thisOnlyRadioButton->isChecked();
}
