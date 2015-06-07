#include "ui_preprocessorgridattributetemplatemappingsettingdialog.h"

#include "preprocessorgridattributetemplatemappingsettingdialog.h"

PreProcessorGridAttributeTemplateMappingSettingDialog::PreProcessorGridAttributeTemplateMappingSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorGridAttributeTemplateMappingSettingDialog)
{
	ui->setupUi(this);
}

PreProcessorGridAttributeTemplateMappingSettingDialog::~PreProcessorGridAttributeTemplateMappingSettingDialog()
{
	delete ui;
}

bool PreProcessorGridAttributeTemplateMappingSettingDialog::autoMode()
{
	return ui->autoRadioButton->isChecked();
}

void PreProcessorGridAttributeTemplateMappingSettingDialog::setAutoMode(bool a)
{
	if (a) {
		ui->autoRadioButton->setChecked(true);
	} else {
		ui->manualRadioButton->setChecked(true);
	}
}

double PreProcessorGridAttributeTemplateMappingSettingDialog::streamWiseLength()
{
	return ui->streamWiseSpinBox->value();
}

void PreProcessorGridAttributeTemplateMappingSettingDialog::setStreamWiseLength(double len)
{
	ui->streamWiseSpinBox->setValue(len);
}

double PreProcessorGridAttributeTemplateMappingSettingDialog::crossStreamLength()
{
	return ui->crossStreamSpinBox->value();
}

void PreProcessorGridAttributeTemplateMappingSettingDialog::setCrossStreamLength(double len)
{
	ui->crossStreamSpinBox->setValue(len);
}

int PreProcessorGridAttributeTemplateMappingSettingDialog::numExpansion()
{
	return ui->numExpansionSpinBox->value();
}

void PreProcessorGridAttributeTemplateMappingSettingDialog::setNumExpansion(int num)
{
	ui->numExpansionSpinBox->setValue(num);
}

double PreProcessorGridAttributeTemplateMappingSettingDialog::weightExponent()
{
	return ui->exponentSpinBox->value();
}

void PreProcessorGridAttributeTemplateMappingSettingDialog::setWeightExponent(double exp)
{
	ui->exponentSpinBox->setValue(exp);
}
