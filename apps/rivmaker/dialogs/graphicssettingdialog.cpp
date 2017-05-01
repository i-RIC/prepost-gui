#include "graphicssettingdialog.h"
#include "ui_graphicssettingdialog.h"

#include "../data/points/pointsgraphicssetting.h"

GraphicsSettingDialog::GraphicsSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GraphicsSettingDialog)
{
	ui->setupUi(this);

	auto& eSetting = PointsGraphicsSetting::elevationPointsSetting;
	auto& weSetting = PointsGraphicsSetting::waterElevationPointsSetting;

	if (eSetting.sizeSetting == PointsGraphicsSetting::SizeSetting::Auto) {
		ui->elevationPointsAutoSizeRadioButton->setChecked(true);
	}	else {
		ui->elevationPointsManualSizeRadioButton->setChecked(true);
	}
	ui->elevationPointsSizeSpinBox->setValue(eSetting.manualSize);
	ui->elevationPointsTransparencySpinBox->setValue(eSetting.transparency);

	if (weSetting.sizeSetting == PointsGraphicsSetting::SizeSetting::Auto) {
		ui->waterElevationPointsAutoSizeRadioButton->setChecked(true);
	}	else {
		ui->waterElevationPointsManualSizeRadioButton->setChecked(true);
	}
	ui->waterElevationPointsSizeSpinBox->setValue(weSetting.manualSize);
	ui->waterElevationPointsTransparencySpinBox->setValue(weSetting.transparency);
}

GraphicsSettingDialog::~GraphicsSettingDialog()
{
	delete ui;
}

void GraphicsSettingDialog::accept()
{
	auto& eSetting = PointsGraphicsSetting::elevationPointsSetting;
	auto& weSetting = PointsGraphicsSetting::waterElevationPointsSetting;

	if (ui->elevationPointsAutoSizeRadioButton->isChecked()) {
		eSetting.sizeSetting = PointsGraphicsSetting::SizeSetting::Auto;
	} else {
		eSetting.sizeSetting = PointsGraphicsSetting::SizeSetting::Manual;
	}
	eSetting.manualSize = ui->elevationPointsSizeSpinBox->value();
	eSetting.transparency = ui->elevationPointsTransparencySpinBox->value();

	if (ui->waterElevationPointsAutoSizeRadioButton->isChecked()) {
		weSetting.sizeSetting = PointsGraphicsSetting::SizeSetting::Auto;
	} else {
		weSetting.sizeSetting = PointsGraphicsSetting::SizeSetting::Manual;
	}
	weSetting.manualSize = ui->waterElevationPointsSizeSpinBox->value();
	weSetting.transparency = ui->waterElevationPointsTransparencySpinBox->value();

	QDialog::accept();
}
