#include "gridcreatingconditionlaplacewholeregiondivisionsettingdialog.h"
#include "ui_gridcreatingconditionlaplacewholeregiondivisionsettingdialog.h"

#include <QPushButton>

GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog)
{
	ui->setupUi(this);
	connect(ui->nISpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateDI()));
	connect(ui->nJSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateDJ()));
}

GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::~GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog()
{
	delete ui;
}

int GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::iDiv() const
{
	return ui->nISpinBox->value();
}

void GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::setIDiv(int div)
{
	ui->nISpinBox->setValue(div);
	updateDI();
}

void GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::setIDivMin(int div)
{
	ui->nISpinBox->setMinimum(div);
	updateDI();
}

int GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::jDiv() const
{
	return ui->nJSpinBox->value();
}

void GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::setJDiv(int div)
{
	ui->nJSpinBox->setValue(div);
	updateDJ();
}

void GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::setJDivMin(int div)
{
	ui->nJSpinBox->setMinimum(div);
	updateDJ();
}

void GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::setILength(double len)
{
	m_iLength = len;
}

void GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::setJLength(double len)
{
	m_jLength = len;
}

void GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::setGridCreateMode()
{
	auto button = ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);
	button->setText(tr("Create &Grid"));
}

void GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::updateDI()
{
	double d = m_iLength / iDiv();
	ui->dIValue->setText(QString::number(d));
}

void GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog::updateDJ()
{
	double d = m_jLength / jDiv();
	ui->dJValue->setText(QString::number(d));
}
