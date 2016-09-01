#include "ui_measureddatavectorsettingdialog.h"
#include "measureddatavectorsettingdialog.h"
#include "private/measureddatavectorsettingdialog_impl.h"

#include <guibase/comboboxtool.h>
#include <guicore/project/measured/measureddata.h>

#include <QtGlobal>

MeasuredDataVectorSettingDialog::Impl::Impl(MeasuredDataVectorSettingDialog* dialog) :
	m_dialog {dialog}
{}

void MeasuredDataVectorSettingDialog::Impl::setupSolutionComboBox(MeasuredData* data)
{
	ComboBoxTool::setupItems(data->scalarNames(), m_dialog->ui->scalarComboBox);
	m_scalars = data->scalarNames();

	ComboBoxTool::setupItems(data->vectorNames(), m_dialog->ui->solutionComboBox);
	m_targets = data->vectorNames();

	if (m_targets.size() <= 1) {
		m_dialog->ui->physValLabel->hide();
		m_dialog->ui->solutionComboBox->hide();
	}

	if (m_scalars.size() == 0) {
		m_dialog->ui->specificRadioButton->isChecked();
		m_dialog->ui->scalarRadioButton->setDisabled(true);
	}
}

// public interfaces

MeasuredDataVectorSettingDialog::MeasuredDataVectorSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::MeasuredDataVectorSettingDialog},
	impl {new Impl(this)}
{
	ui->setupUi(this);
}

MeasuredDataVectorSettingDialog::~MeasuredDataVectorSettingDialog()
{
	delete ui;
	delete impl;
}

void MeasuredDataVectorSettingDialog::setData(MeasuredData* data)
{
	impl->setupSolutionComboBox(data);
}

MeasuredDataVectorSetting MeasuredDataVectorSettingDialog::setting() const
{
	MeasuredDataVectorSetting ret = impl->m_setting;

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
	impl->m_setting = setting;

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

