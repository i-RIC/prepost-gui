#include "ui_measureddatavectorsettingdialog.h"

#include "measureddatavectorsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guicore/project/measured/measureddata.h>

#include <QtGlobal>

MeasuredDataVectorSettingDialog::MeasuredDataVectorSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::MeasuredDataVectorSettingDialog}
{
	ui->setupUi(this);
}

MeasuredDataVectorSettingDialog::~MeasuredDataVectorSettingDialog()
{
	delete ui;
}

void MeasuredDataVectorSettingDialog::setData(MeasuredData* data)
{
	setupSolutionComboBox(data);
}

MeasuredDataVectorSetting MeasuredDataVectorSettingDialog::setting() const
{
	MeasuredDataVectorSetting ret = m_setting;

	ret.target = ui->solutionComboBox->currentText();

	if (ui->specificRadioButton->isChecked()) {
		ret.colorMode = ArrowSettingContainer::ColorMode::Custom;
	} else {
		ret.colorMode = ArrowSettingContainer::ColorMode::ByScalar;
	}
	ret.customColor = ui->colorEditWidget->color();
	ret.colorTarget = ui->scalarComboBox->currentText();

	if (ui->lengthAutoCheckBox->isChecked()) {
		ret.lengthMode = ArrowSettingContainer::LengthMode::Auto;
	} else {
		ret.lengthMode = ArrowSettingContainer::LengthMode::Custom;
	}
	ret.standardValue = ui->stdValueSpinBox->value();
	ret.legendLength = ui->legendLengthSpinBox->value();
	ret.minimumValue = ui->minValueSpinBox->value();

	return ret;
}

void MeasuredDataVectorSettingDialog::setSetting(const MeasuredDataVectorSetting& setting)
{
	m_setting = setting;

	ui->solutionComboBox->setCurrentText(setting.target);

	if (setting.colorMode == ArrowSettingContainer::ColorMode::Custom) {
		ui->specificRadioButton->setChecked(true);
	} else {
		ui->scalarRadioButton->setChecked(true);
	}
	ui->colorEditWidget->setColor(setting.customColor);
	ui->scalarComboBox->setCurrentText(setting.colorTarget);

	if (setting.lengthMode == ArrowSettingContainer::LengthMode::Auto) {
		ui->lengthAutoCheckBox->setChecked(true);
	} else {
		ui->lengthAutoCheckBox->setChecked(false);
	}
	ui->stdValueSpinBox->setValue(setting.standardValue);
	ui->legendLengthSpinBox->setValue(setting.legendLength);
	ui->minValueSpinBox->setValue(setting.minimumValue);
}

void MeasuredDataVectorSettingDialog::setupSolutionComboBox(MeasuredData* data)
{
	ComboBoxTool::setupItems(data->scalarNames(), ui->scalarComboBox);
	m_scalars = data->scalarNames();

	ComboBoxTool::setupItems(data->vectorNames(), ui->solutionComboBox);
	m_targets = data->vectorNames();

	if (m_targets.size() <= 1) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}

	if (m_scalars.size() == 0) {
		ui->specificRadioButton->isChecked();
		ui->scalarRadioButton->setDisabled(true);
	}
}
