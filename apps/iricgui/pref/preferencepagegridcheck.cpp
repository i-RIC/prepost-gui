#include "ui_preferencepagegridcheck.h"

#include "preferencepagegridcheck.h"

PreferencePageGridCheck::PreferencePageGridCheck(QWidget* parent) :
	PreferencePage(parent),
	ui(new Ui::PreferencePageGridCheck)
{
	ui->setupUi(this);
	m_settings.beginGroup("gridcheck");

	ui->beforeExecCheckBox->setChecked(m_settings.value("beforeexec", true).value<bool>());
	ui->beforeExportCheckBox->setChecked(m_settings.value("beforeexport", true).value<bool>());

	ui->cellCrushCheckBox->setChecked(m_settings.value("checkcrushed", true).value<bool>());

	ui->aspectRatioCheckBox->setChecked(m_settings.value("checkaspectratio", false).value<bool>());
	ui->aspectRatioSpinBox->setValue(m_settings.value("aspectratiolimit", 1.5).value<double>());

	ui->variationCheckBox->setChecked(m_settings.value("checkvariation", false).value<bool>());
	ui->variationRatioISpinBox->setValue(m_settings.value("variationilimit", 1.5).value<double>());
	ui->variationRatioJSpinBox->setValue(m_settings.value("variationjlimit", 1.5).value<double>());

	ui->angleCheckBox->setChecked(m_settings.value("checkangle", false).value<bool>());
	ui->angleSpinBox->setValue(m_settings.value("anglelimit", 40).value<double>());
}

PreferencePageGridCheck::~PreferencePageGridCheck()
{
	delete ui;
}

void PreferencePageGridCheck::update()
{
	m_settings.setValue("beforeexec", ui->beforeExecCheckBox->isChecked());
	m_settings.setValue("beforeexport", ui->beforeExportCheckBox->isChecked());

	m_settings.setValue("checkcrushed", ui->cellCrushCheckBox->isChecked());

	m_settings.setValue("checkaspectratio", ui->aspectRatioCheckBox->isChecked());
	m_settings.setValue("aspectratiolimit", ui->aspectRatioSpinBox->value());

	m_settings.setValue("checkvariation", ui->variationCheckBox->isChecked());
	m_settings.setValue("variationilimit", ui->variationRatioISpinBox->value());
	m_settings.setValue("variationjlimit", ui->variationRatioJSpinBox->value());

	m_settings.setValue("checkangle", ui->angleCheckBox->isChecked());
	m_settings.setValue("anglelimit", ui->angleSpinBox->value());
}
