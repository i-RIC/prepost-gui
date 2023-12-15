#include "ui_gridcreatingconditioncompoundchannelsettingdialog.h"

#include "gridcreatingconditioncompoundchannelsettingdialog.h"

#include <QPushButton>

GridCreatingConditionCompoundChannelSettingDialog::GridCreatingConditionCompoundChannelSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionCompoundChannelSettingDialog)
{
	ui->setupUi(this);

	ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("&Create Grid"));
}

GridCreatingConditionCompoundChannelSettingDialog::~GridCreatingConditionCompoundChannelSettingDialog()
{
	delete ui;
}

void GridCreatingConditionCompoundChannelSettingDialog::setStreamWiseDivision(int div)
{
	ui->streamWiseSpinBox->setValue(div);
}

void GridCreatingConditionCompoundChannelSettingDialog::setLeftDivision(int div)
{
	ui->leftSpinBox->setValue(div);
}

void GridCreatingConditionCompoundChannelSettingDialog::setRightDivision(int div)
{
	ui->rightSpinBox->setValue(div);
}

void GridCreatingConditionCompoundChannelSettingDialog::setCenterDivision(int div)
{
	ui->centerSpinBox->setValue(div);
}

void GridCreatingConditionCompoundChannelSettingDialog::setRelaxation(double r)
{
	ui->relaxationSpinBox->setValue(r);
}

void GridCreatingConditionCompoundChannelSettingDialog::setIterations(int i)
{
	ui->iterationSpinBox->setValue(i);
}

int GridCreatingConditionCompoundChannelSettingDialog::streamWiseDivision()
{
	return ui->streamWiseSpinBox->value();
}

int GridCreatingConditionCompoundChannelSettingDialog::leftDivision()
{
	return ui->leftSpinBox->value();
}

int GridCreatingConditionCompoundChannelSettingDialog::rightDivision()
{
	return ui->rightSpinBox->value();
}

int GridCreatingConditionCompoundChannelSettingDialog::centerDivision()
{
	return ui->centerSpinBox->value();
}

double GridCreatingConditionCompoundChannelSettingDialog::relaxation()
{
	return ui->relaxationSpinBox->value();
}

int GridCreatingConditionCompoundChannelSettingDialog::iterations()
{
	return ui->iterationSpinBox->value();
}

void GridCreatingConditionCompoundChannelSettingDialog::toggleDetailCondition()
{
	bool newDetailVisible = ! ui->optimizationGroupBox->isVisible();
	ui->optimizationGroupBox->setVisible(newDetailVisible);
	adjustSize();
}
