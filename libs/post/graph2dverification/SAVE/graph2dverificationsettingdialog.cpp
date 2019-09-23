#include "ui_graph2dverificationsettingdialog.h"

#include "graph2dverificationsettingdialog.h"

#include <QMessageBox>

#define RANGEMIN 1E-8

Graph2dVerificationSettingDialog::Graph2dVerificationSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dVerificationSettingDialog)
{
	ui->setupUi(this);
	connect(ui->xAxisTimeRadioButton, SIGNAL(toggled(bool)), this, SLOT(xAxisRadioButtonToggled()));
	connect(ui->xAxisTimestepRadioButton, SIGNAL(toggled(bool)), this, SLOT(xAxisRadioButtonToggled()));
	connect(ui->xAxisDistanceRadioButton, SIGNAL(toggled(bool)), this, SLOT(xAxisRadioButtonToggled()));
	connect(ui->xAxisGridindexRadioButton, SIGNAL(toggled(bool)), this, SLOT(xAxisRadioButtonToggled()));

	connect(ui->yAxisSideComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(yAxisComboBoxChange(int)));
}

Graph2dVerificationSettingDialog::~Graph2dVerificationSettingDialog()
{
	delete ui;
}

void Graph2dVerificationSettingDialog::setSetting(const Graph2dVerificationWindowResultSetting& setting)
{
	m_setting = setting;
	ui->xAxisReverseCheckBox->setChecked(m_setting.xAxisReverse());
	if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
		ui->xAxisDistanceGroupBox->hide();
		ui->xAxisReverseCheckBox->hide();
		if (m_setting.timeValueType() == Graph2dVerificationWindowResultSetting::tvtTime) {
			ui->xAxisTimeRadioButton->setChecked(true);
		} else {
			ui->xAxisTimestepRadioButton->setChecked(true);
		}
	} else {
		ui->xAxisTimeGroupBox->hide();
		if (m_setting.positionValueType() == Graph2dVerificationWindowResultSetting::pvtDistance) {
			ui->xAxisDistanceRadioButton->setChecked(true);
		} else {
			ui->xAxisGridindexRadioButton->setChecked(true);
		}
	}
	ui->xAxisLabelEdit->setText(setting.xAxisLabel());
	ui->xAxisValueRangeAutoCheckBox->setChecked(setting.xAxisAutoRange());
	ui->xAxisMinEdit->setValue(setting.xAxisValueMin());
	ui->xAxisMaxEdit->setValue(setting.xAxisValueMax());
	ui->xAxisLogCheckBox->setChecked(setting.xAxisLog());

	applyYAxisLeftSetting();

	ui->titleEdit->setText(m_setting.title());
	ui->showTimeCheckBox->setChecked(m_setting.addIndicesToTitle());
}

void Graph2dVerificationSettingDialog::accept()
{
	applyXAxisSetting();
	if (ui->yAxisSideComboBox->currentIndex() == 0) {
		saveYAxisLeftSetting();
	} else {
		saveYAxisRightSetting();
	}
	m_setting.setTitle(ui->titleEdit->text());
	m_setting.setAddIndicesToTitle(ui->showTimeCheckBox->isChecked());

	if (! axisRangesCheck()) {
		return;
	}
	QDialog::accept();
}

void Graph2dVerificationSettingDialog::xAxisRadioButtonToggled()
{
	applyXAxisSetting();
	m_setting.setAutoXAxisLabel();
	ui->xAxisLabelEdit->setText(m_setting.xAxisLabel());
}

void Graph2dVerificationSettingDialog::applyYAxisLeftSetting()
{
	ui->yAxisValueRangeAutoCheckBox->setChecked(m_setting.yAxisLeftAutoRange());
	ui->yAxisMinEdit->setValue(m_setting.yAxisLeftMin());
	ui->yAxisMaxEdit->setValue(m_setting.yAxisLeftMax());
	ui->yAxisLabelEdit->setText(m_setting.yAxisLeftTitle());
	ui->yAxisReverseCheckBox->setChecked(m_setting.yAxisLeftReverse());
	ui->yAxisLogCheckBox->setChecked(m_setting.yAxisLeftLog());
}

void Graph2dVerificationSettingDialog::applyYAxisRightSetting()
{
	ui->yAxisValueRangeAutoCheckBox->setChecked(m_setting.yAxisRightAutoRange());
	ui->yAxisMinEdit->setValue(m_setting.yAxisRightMin());
	ui->yAxisMaxEdit->setValue(m_setting.yAxisRightMax());
	ui->yAxisLabelEdit->setText(m_setting.yAxisRightTitle());
	ui->yAxisReverseCheckBox->setChecked(m_setting.yAxisRightReverse());
	ui->yAxisLogCheckBox->setChecked(m_setting.yAxisRightLog());
}

void Graph2dVerificationSettingDialog::saveYAxisLeftSetting()
{
	m_setting.setYAxisLeftAutoRange(ui->yAxisValueRangeAutoCheckBox->isChecked());
	m_setting.setYAxisLeftMin(ui->yAxisMinEdit->value());
	m_setting.setYAxisLeftMax(ui->yAxisMaxEdit->value());
	m_setting.setYAxisLeftTitle(ui->yAxisLabelEdit->text());
	m_setting.setYAxisLeftReverse(ui->yAxisReverseCheckBox->isChecked());
	m_setting.setYAxisLeftLog(! ui->yAxisValueRangeAutoCheckBox->isChecked() && ui->yAxisLogCheckBox->isChecked());
}

void Graph2dVerificationSettingDialog::saveYAxisRightSetting()
{
	m_setting.setYAxisRightAutoRange(ui->yAxisValueRangeAutoCheckBox->isChecked());
	m_setting.setYAxisRightMin(ui->yAxisMinEdit->value());
	m_setting.setYAxisRightMax(ui->yAxisMaxEdit->value());
	m_setting.setYAxisRightTitle(ui->yAxisLabelEdit->text());
	m_setting.setYAxisRightReverse(ui->yAxisReverseCheckBox->isChecked());
	m_setting.setYAxisRightLog(! ui->yAxisValueRangeAutoCheckBox->isChecked() && ui->yAxisLogCheckBox->isChecked());
}

void Graph2dVerificationSettingDialog::yAxisComboBoxChange(int index)
{
	if (index == 0) {
		// save right setting.
		saveYAxisRightSetting();
		// apply left setting.
		applyYAxisLeftSetting();
	} else {
		// save left setting.
		saveYAxisLeftSetting();
		// apply right setting.
		applyYAxisRightSetting();
	}
}

void Graph2dVerificationSettingDialog::applyXAxisSetting()
{
	if (ui->xAxisTimeRadioButton->isChecked()) {
		m_setting.setTimeValueType(Graph2dVerificationWindowResultSetting::tvtTime);
	} else {
		m_setting.setTimeValueType(Graph2dVerificationWindowResultSetting::tvtTimeStep);
	}
	if (ui->xAxisDistanceRadioButton->isChecked()) {
		m_setting.setPositionValueType(Graph2dVerificationWindowResultSetting::pvtDistance);
	} else {
		m_setting.setPositionValueType(Graph2dVerificationWindowResultSetting::pvtIndex);
	}
	if (! ui->xAxisValueRangeAutoCheckBox->isChecked()) {
		m_setting.setXAxisValueMin(ui->xAxisMinEdit->value());
		m_setting.setXAxisValueMax(ui->xAxisMaxEdit->value());
	}
	m_setting.setXAxisLabel(ui->xAxisLabelEdit->text());
	m_setting.setXAxisReverse(ui->xAxisReverseCheckBox->isChecked());
	m_setting.setXAxisLog(! ui->xAxisValueRangeAutoCheckBox->isChecked() && ui->xAxisLogCheckBox->isChecked());

	m_setting.setXAxisAutoRange(ui->xAxisValueRangeAutoCheckBox->isChecked());
}

bool Graph2dVerificationSettingDialog::axisRangesCheck()
{
	// X axis range check
	if (! m_setting.xAxisAutoRange()) {
		if (m_setting.xAxisValueMax() - m_setting.xAxisValueMin() < RANGEMIN) {
			QMessageBox::warning(this, tr("Warning"), tr("X-axes range is invalid."));
			return false;
		} else if (m_setting.xAxisLog() && m_setting.xAxisValueMin() <= 0) {
			QMessageBox::warning(this, tr("Warning"), tr("X-axes range is invalid."));
			return false;
		}
	}
	if (! m_setting.yAxisLeftAutoRange()) {
		if (m_setting.yAxisLeftMax() - m_setting.yAxisLeftMin() < RANGEMIN) {
			QMessageBox::warning(this, tr("Warning"), tr("Left Y-axes range is invalid."));
			return false;
		} else if (m_setting.yAxisLeftLog() && m_setting.yAxisLeftMin() <= 0) {
			QMessageBox::warning(this, tr("Warning"), tr("Left Y-axes range is invalid."));
			return false;
		}
	}
	if (! m_setting.yAxisRightAutoRange()) {
		if (m_setting.yAxisRightMax() - m_setting.yAxisRightMin() < RANGEMIN) {
			QMessageBox::warning(this, tr("Warning"), tr("Right Y-axes range is invalid."));
			return false;
		} else if (m_setting.yAxisRightLog() && m_setting.yAxisRightMin() <= 0) {
			QMessageBox::warning(this, tr("Warning"), tr("Right Y-axes range is invalid."));
			return false;
		}
	}
	return true;
}
