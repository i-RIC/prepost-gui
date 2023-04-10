#include "geodatapointmap_mappingsetting.h"
#include "geodatapointmap_mappingsettingdialog_templatedetaildialog.h"
#include "ui_geodatapointmap_mappingsettingdialog_templatedetaildialog.h"

GeoDataPointmap::MappingSettingDialog::TemplateDetailDialog::TemplateDetailDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmap_MappingSettingDialog_TemplateDetailDialog)
{
	ui->setupUi(this);
}

GeoDataPointmap::MappingSettingDialog::TemplateDetailDialog::~TemplateDetailDialog()
{
	delete ui;
}

GeoDataPointmap::MappingSetting GeoDataPointmap::MappingSettingDialog::TemplateDetailDialog::setting() const
{
	MappingSetting setting;

	setting.templateAutoMode = ui->autoRadioButton->isChecked();
	setting.templateStreamWiseLength = ui->streamWiseSpinBox->value();
	setting.templateCrossStreamLength = ui->crossStreamSpinBox->value();
	setting.templateNumberOfExpansions = ui->numExpansionSpinBox->value();
	setting.templateWeightExponent = ui->exponentSpinBox->value();

	return setting;
}

void GeoDataPointmap::MappingSettingDialog::TemplateDetailDialog::setSetting(const MappingSetting& setting)
{
	if (setting.templateAutoMode) {
		ui->autoRadioButton->setChecked(true);
	} else {
		ui->manualRadioButton->setChecked(true);
	}
	ui->streamWiseSpinBox->setValue(setting.templateStreamWiseLength);
	ui->crossStreamSpinBox->setValue(setting.templateCrossStreamLength);
	ui->numExpansionSpinBox->setValue(setting.templateNumberOfExpansions);
	ui->exponentSpinBox->setValue(setting.templateWeightExponent);
}
