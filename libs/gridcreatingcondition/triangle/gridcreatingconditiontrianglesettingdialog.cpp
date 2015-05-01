#include "ui_gridcreatingconditiontrianglesettingdialog.h"

#include "gridcreatingconditiontrianglesettingdialog.h"

GridCreatingConditionTriangleSettingDialog::GridCreatingConditionTriangleSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionTriangleSettingDialog)
{
	ui->setupUi(this);
	ui->angleSpinBox->setValue(30);
	ui->areaEdit->setValue(1000);
}

GridCreatingConditionTriangleSettingDialog::~GridCreatingConditionTriangleSettingDialog()
{
	delete ui;
}

bool GridCreatingConditionTriangleSettingDialog::angleConstraint()
{
	return ui->angleCheckBox->isChecked();
}

void GridCreatingConditionTriangleSettingDialog::setAngleConstraint(bool c)
{
	ui->angleCheckBox->setChecked(c);
}

double GridCreatingConditionTriangleSettingDialog::angle()
{
	return ui->angleSpinBox->value();
}

void GridCreatingConditionTriangleSettingDialog::setAngle(double angle)
{
	ui->angleSpinBox->setValue(angle);
}

bool GridCreatingConditionTriangleSettingDialog::areaConstraint()
{
	return ui->areaCheckBox->isChecked();
}

void GridCreatingConditionTriangleSettingDialog::setAreaConstraint(bool c)
{
	ui->areaCheckBox->setChecked(c);
}

double GridCreatingConditionTriangleSettingDialog::area()
{
	return ui->areaEdit->value();
}

void GridCreatingConditionTriangleSettingDialog::setArea(double a)
{
	ui->areaEdit->setValue(a);
}
