#include "post2dwindowgraphsettingdialog.h"
#include "ui_post2dwindowgraphsettingdialog.h"
#include "private/post2dwindowgraphsettingcustomregiondialog.h"
#include "private/post2dwindowgraphsettingdialog_impl.h"

#include <guibase/comboboxtool.h>

Post2dWindowGraphSettingDialog::Post2dWindowGraphSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowGraphSettingDialog),
	impl {new Impl()}
{
	ui->setupUi(this);

	connect(ui->iDirRadioButton, SIGNAL(toggled(bool)), this, SLOT(handleDirChange(bool)));
	connect(ui->jDirRadioButton, SIGNAL(toggled(bool)), this, SLOT(handleDirChange(bool)));

	connect(ui->startSlider, SIGNAL(valueChanged(int)), this, SLOT(handleStartChange(int)));
	connect(ui->endSlider, SIGNAL(valueChanged(int)), this, SLOT(handleEndChange(int)));

	connect(ui->regionCustomSettingPushButton, SIGNAL(clicked(bool)), this, SLOT(editRegionCustomSetting()));
}

Post2dWindowGraphSettingDialog::~Post2dWindowGraphSettingDialog()
{
	delete ui;
}

void Post2dWindowGraphSettingDialog::setDimensions(int dimI, int dimJ)
{
	impl->m_dimI = dimI;
	impl->m_dimJ = dimJ;
}

void Post2dWindowGraphSettingDialog::setTargets(const std::vector<std::string>& targets)
{
	ComboBoxTool::setupItems(targets, ui->targetComboBox);
}

Post2dWindowGraphSetting Post2dWindowGraphSettingDialog::setting() const
{
	Post2dWindowGraphSetting ret = impl->m_setting;

	if (ui->iDirRadioButton->isChecked()) {
		ret.direction = Post2dWindowGraphSetting::dirI;
	} else {
		ret.direction = Post2dWindowGraphSetting::dirJ;
	}

	if (ui->regionAllRadioButton->isChecked()) {
		ret.region = Post2dWindowGraphSetting::All;
	} else if (ui->regionPartialRadioButton->isChecked()) {
		ret.region = Post2dWindowGraphSetting::Partial;
		ret.regionStartIndex = ui->startSlider->value();
		ret.regionEndIndex = ui->endSlider->value();
		ret.regionSkipIndex = ui->skipSpinBox->value();
	} else {
		ret.region = Post2dWindowGraphSetting::Custom;
	}

	if (ui->dirVerticalRadioButton->isChecked()) {
		ret.offsetDirection = Post2dWindowGraphSetting::Vertical;
	} else {
		ret.offsetDirection = Post2dWindowGraphSetting::Axis;
	}

	ret.drawGridLine = ui->drawGridLinesCheckBox->isChecked();
	ret.gridLineStyle = ui->gridLineStyleWidget->lineStyle();

	ret.graphTarget = ui->targetComboBox->currentText();
	ret.graphScale = ui->scaleSpinBox->value();
	ret.graphLineStyle = ui->graphLineStyleWidget->lineStyle();

	if (ui->fixNofixRadioButton->isChecked()) {
		ret.graphValueFix = Post2dWindowGraphSetting::NoFix;
	} else if (ui->fixSubMaxRadioButton->isChecked()) {
		ret.graphValueFix = Post2dWindowGraphSetting::SubtractMax;
	} else {
		ret.graphValueFix = Post2dWindowGraphSetting::SubtractMin;
	}

	return ret;
}

void Post2dWindowGraphSettingDialog::setSetting(const Post2dWindowGraphSetting& setting)
{
	impl->m_setting = setting;

	if (setting.direction == Post2dWindowGraphSetting::dirI) {
		ui->iDirRadioButton->setChecked(true);
	} else {
		ui->jDirRadioButton->setChecked(true);
	}
	updateStartEndValueRange();

	ui->startSlider->setValue(0);
	ui->endSlider->setValue(ui->endSlider->maximum());
	if (setting.region == Post2dWindowGraphSetting::All) {
		ui->regionAllRadioButton->setChecked(true);
	} else if (setting.region == Post2dWindowGraphSetting::Partial) {
		ui->regionPartialRadioButton->setChecked(true);
		ui->startSlider->setValue(setting.regionStartIndex);
		ui->endSlider->setValue(setting.regionEndIndex);
		ui->skipSpinBox->setValue(setting.regionSkipIndex);
	} else {
		ui->regionCustomRadioButton->setChecked(true);
	}

	if (setting.offsetDirection == Post2dWindowGraphSetting::Axis) {
		ui->dirParallelRadioButton->setChecked(true);
	} else {
		ui->dirVerticalRadioButton->setChecked(true);
	}

	ui->drawGridLinesCheckBox->setChecked(setting.drawGridLine);
	ui->gridLineStyleWidget->setLineStyle(setting.gridLineStyle);

	ui->targetComboBox->setCurrentText(setting.graphTarget);
	ui->scaleSpinBox->setValue(setting.graphScale);
	ui->graphLineStyleWidget->setLineStyle(setting.graphLineStyle);

	if (setting.graphValueFix == Post2dWindowGraphSetting::NoFix) {
		ui->fixNofixRadioButton->setChecked(true);
	} else if (setting.graphValueFix == Post2dWindowGraphSetting::SubtractMax) {
		ui->fixSubMaxRadioButton->setChecked(true);
	} else {
		ui->fixSubMinRadioButton->setChecked(true);
	}
}

void Post2dWindowGraphSettingDialog::handleDirChange(bool checked)
{
	if (checked == false) {return;}

	updateStartEndValueRange();
	resetRegionCustomSetting();
}

void Post2dWindowGraphSettingDialog::handleStartChange(int val)
{
	if (ui->endSlider->value() < val){
		ui->endSlider->setValue(val);
	}
}

void Post2dWindowGraphSettingDialog::handleEndChange(int val)
{
	if (ui->startSlider->value() > val) {
		ui->startSlider->setValue(val);
	}
}

void Post2dWindowGraphSettingDialog::editRegionCustomSetting()
{
	Post2dWindowGraphSettingCustomRegionDialog dialog(this);

	if (ui->iDirRadioButton->isChecked()) {
		dialog.setIndexCount(impl->m_dimI);
	} else {
		dialog.setIndexCount(impl->m_dimJ);
	}
	dialog.setSetting(impl->m_setting.regionIndices);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	impl->m_setting.regionIndices = dialog.setting();
}

void Post2dWindowGraphSettingDialog::updateStartEndValueRange()
{
	if (ui->iDirRadioButton->isChecked()) {
		ui->startSlider->setRange(0, impl->m_dimI - 1);
		ui->endSlider->setRange(0, impl->m_dimI - 1);
	} else {
		ui->startSlider->setRange(0, impl->m_dimJ - 1);
		ui->endSlider->setRange(0, impl->m_dimJ - 1);
	}
}

void Post2dWindowGraphSettingDialog::resetRegionCustomSetting()
{
	int size = 0;
	if (ui->iDirRadioButton->isChecked()) {
		size = impl->m_dimI;
	} else {
		size = impl->m_dimJ;
	}
	std::vector<int> indices(size);
	for (int i = 0; i < size; ++i) {
		indices[i] = i;
	}
	impl->m_setting.regionIndices = indices;
}
