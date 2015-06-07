#include "ui_graph2dwindowverticalaxissettingdialog.h"

#include "graph2dwindowverticalaxissettingdialog.h"

Graph2dWindowVerticalAxisSettingDialog::Graph2dWindowVerticalAxisSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dWindowVerticalAxisSettingDialog)
{
	ui->setupUi(this);
}

Graph2dWindowVerticalAxisSettingDialog::~Graph2dWindowVerticalAxisSettingDialog()
{
	delete ui;
}

void Graph2dWindowVerticalAxisSettingDialog::setSetting(const Graph2dWindowVerticalAxisSetting& setting)
{
	m_setting = setting;
	if (m_setting.rangeMode() == Graph2dWindowVerticalAxisSetting::rmAuto) {
		ui->autoRangeCheckBox->setChecked(true);
		ui->rangeMinLineEdit->setValue(m_autoRangeMin);
		ui->rangeMaxLineEdit->setValue(m_autoRangeMax);
	} else {
		ui->autoRangeCheckBox->setChecked(false);
		ui->rangeMinLineEdit->setValue(m_setting.customRangeMin());
		ui->rangeMaxLineEdit->setValue(m_setting.customRangeMax());
	}
	if (m_setting.labelMode() == Graph2dWindowVerticalAxisSetting::lmAuto) {
		ui->labelAutoRadioButton->setChecked(true);
		ui->labelCustomLineEdit->setEnabled(false);
		ui->labelCustomLineEdit->setText("");
	} else {
		ui->labelCustomRadioButton->setChecked(true);
		ui->labelCustomLineEdit->setText(m_setting.customLabel());
	}
}

void Graph2dWindowVerticalAxisSettingDialog::accept()
{
	if (ui->autoRangeCheckBox->isChecked()) {
		m_setting.setRangeMode(Graph2dWindowVerticalAxisSetting::rmAuto);
	} else {
		m_setting.setRangeMode(Graph2dWindowVerticalAxisSetting::rmCustom);
		m_setting.setCustomRange(ui->rangeMinLineEdit->value(), ui->rangeMaxLineEdit->value());
	}
	if (ui->labelAutoRadioButton->isChecked()) {
		m_setting.setLabelMode(Graph2dWindowVerticalAxisSetting::lmAuto);
	} else {
		m_setting.setLabelMode(Graph2dWindowVerticalAxisSetting::lmCustom);
		m_setting.setCustomLabel(ui->labelCustomLineEdit->text());
	}
	QDialog::accept();
}

void Graph2dWindowVerticalAxisSettingDialog::setAutoRange(double min, double max)
{
	m_autoRangeMin = min;
	m_autoRangeMax = max;
}
