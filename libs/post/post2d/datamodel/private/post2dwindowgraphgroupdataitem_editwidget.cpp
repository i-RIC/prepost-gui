#include "post2dwindowgraphgroupdataitem_editwidget.h"
#include "post2dwindowgraphgroupdataitem_editwidget_customregiondialog.h"
#include "post2dwindowgraphgroupdataitem_impl.h"
#include "ui_post2dwindowgraphgroupdataitem_editwidget.h"

#include <guibase/comboboxtool.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowGraphGroupDataItem::EditWidget::EditWidget(Post2dWindowGraphGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_item {item},
	ui(new Ui::Post2dWindowGraphGroupDataItem_EditWidget)
{
	ui->setupUi(this);

	connect(ui->iDirRadioButton, &QRadioButton::toggled, this, &EditWidget::handleDirChange);
	connect(ui->jDirRadioButton, &QRadioButton::toggled, this, &EditWidget::handleDirChange);

	connect(ui->startSlider, &SliderWithValue::valueChanged, this, &EditWidget::handleStartChange);
	connect(ui->endSlider, &SliderWithValue::valueChanged, this, &EditWidget::handleEndChange);

	connect(ui->regionCustomSettingPushButton, &QPushButton::clicked, this, &EditWidget::editRegionCustomSetting);
}

Post2dWindowGraphGroupDataItem::EditWidget::~EditWidget()
{
	delete ui;
}

void Post2dWindowGraphGroupDataItem::EditWidget::setDimensions(int dimI, int dimJ)
{
	m_dimI = dimI;
	m_dimJ = dimJ;
}

void Post2dWindowGraphGroupDataItem::EditWidget::setTargets(const std::vector<std::string>& targets)
{
	ComboBoxTool::setupItems(targets, ui->targetComboBox);
}

Post2dWindowGraphSetting Post2dWindowGraphGroupDataItem::EditWidget::setting() const
{
	Post2dWindowGraphSetting ret = m_setting;

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

void Post2dWindowGraphGroupDataItem::EditWidget::setSetting(const Post2dWindowGraphSetting& setting)
{
	m_setting = setting;

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

QUndoCommand* Post2dWindowGraphGroupDataItem::EditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<Post2dWindowGraphSetting>(iRIC::generateCommandId("Post2dWindowGraphGroupDataItem::EditWidget"), apply, setting(), &m_item->impl->m_setting);
}

void Post2dWindowGraphGroupDataItem::EditWidget::handleDirChange(bool checked)
{
	if (checked == false) {return;}

	updateStartEndValueRange();
	resetRegionCustomSetting();
}

void Post2dWindowGraphGroupDataItem::EditWidget::handleStartChange(int val)
{
	if (ui->endSlider->value() < val){
		ui->endSlider->setValue(val);
	}
}

void Post2dWindowGraphGroupDataItem::EditWidget::handleEndChange(int val)
{
	if (ui->startSlider->value() > val) {
		ui->startSlider->setValue(val);
	}
}

void Post2dWindowGraphGroupDataItem::EditWidget::editRegionCustomSetting()
{
	CustomRegionDialog dialog(this);

	if (ui->iDirRadioButton->isChecked()) {
		dialog.setIndexCount(m_dimI);
	} else {
		dialog.setIndexCount(m_dimJ);
	}
	dialog.setSetting(m_setting.regionIndices);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.regionIndices = dialog.setting();
}

void Post2dWindowGraphGroupDataItem::EditWidget::updateStartEndValueRange()
{
	if (ui->iDirRadioButton->isChecked()) {
		ui->startSlider->setRange(0, m_dimI - 1);
		ui->endSlider->setRange(0, m_dimI - 1);
	} else {
		ui->startSlider->setRange(0, m_dimJ - 1);
		ui->endSlider->setRange(0, m_dimJ - 1);
	}
}
void Post2dWindowGraphGroupDataItem::EditWidget::resetRegionCustomSetting()
{
	int size = 0;
	if (ui->iDirRadioButton->isChecked()) {
		size = m_dimI;
	} else {
		size = m_dimJ;
	}
	std::vector<int> indices(size);
	for (int i = 0; i < size; ++i) {
		indices[i] = i;
	}
	m_setting.regionIndices = indices;
}
