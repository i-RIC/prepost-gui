#include "xyaxisdisplaysettingdialog.h"
#include "ui_xyaxisdisplaysettingdialog.h"

XYAxisDisplaySettingDialog::XYAxisDisplaySettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XYAxisDisplaySettingDialog)
{
    ui->setupUi(this);
}

XYAxisDisplaySettingDialog::~XYAxisDisplaySettingDialog()
{
    delete ui;
}

void XYAxisDisplaySettingDialog::setXAxisSetting(const Setting& xSetting)
{
	ui->xAutoCheckBox->setChecked(xSetting.isAuto);
	ui->xMinEdit->setValue(xSetting.min);
	ui->xMaxEdit->setValue(xSetting.max);
	ui->xNumLabelsSlider->setValue(xSetting.numOfLabels);
}

const XYAxisDisplaySettingDialog::Setting XYAxisDisplaySettingDialog::xAxisSetting()
{
	Setting s;
	s.isAuto = ui->xAutoCheckBox->isChecked();
	s.min = ui->xMinEdit->value();
	s.max = ui->xMaxEdit->value();
	s.numOfLabels = ui->xNumLabelsSlider->value();
	return s;
}

void XYAxisDisplaySettingDialog::setYAxisSetting(const Setting& ySetting)
{
	ui->yAutoCheckBox->setChecked(ySetting.isAuto);
	ui->yMinEdit->setValue(ySetting.min);
	ui->yMaxEdit->setValue(ySetting.max);
	ui->yNumLabelsSlider->setValue(ySetting.numOfLabels);
}

const XYAxisDisplaySettingDialog::Setting XYAxisDisplaySettingDialog::yAxisSetting()
{
	Setting s;
	s.isAuto = ui->yAutoCheckBox->isChecked();
	s.min = ui->yMinEdit->value();
	s.max = ui->yMaxEdit->value();
	s.numOfLabels = ui->yNumLabelsSlider->value();
	return s;
}
