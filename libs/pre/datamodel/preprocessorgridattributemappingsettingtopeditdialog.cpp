#include "ui_preprocessorgridattributemappingsettingtopeditdialog.h"

#include "preprocessorgridattributemappingsettingtopeditdialog.h"
#include "preprocessorgridattributetemplatemappingsettingdialog.h"

PreProcessorGridAttributeMappingSettingTopEditDialog::PreProcessorGridAttributeMappingSettingTopEditDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::PreProcessorGridAttributeMappingSettingTopEditDialog}
{
	ui->setupUi(this);
}

PreProcessorGridAttributeMappingSettingTopEditDialog::~PreProcessorGridAttributeMappingSettingTopEditDialog()
{
	delete ui;
}

PreProcessorGridAttributeMappingMode::Mode PreProcessorGridAttributeMappingSettingTopEditDialog::mappingMode()
{
	if (ui->autoRadioButton->isChecked()) {
		return PreProcessorGridAttributeMappingMode::mAuto;
	} else {
		return PreProcessorGridAttributeMappingMode::mManual;
	}

}

void PreProcessorGridAttributeMappingSettingTopEditDialog::setMappingMode(PreProcessorGridAttributeMappingMode::Mode mm)
{
	if (mm == PreProcessorGridAttributeMappingMode::mAuto) {
		ui->autoRadioButton->setChecked(true);
	} else {
		ui->manualRadioButton->setChecked(true);
	}
}
