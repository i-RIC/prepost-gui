#include "ui_post2dwindowarrowstructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowarrowstructuredsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QtGlobal>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowArrowStructuredSettingDialog::Post2dWindowArrowStructuredSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowArrowStructuredSettingDialog)
{
	ui->setupUi(this);
	connect(ui->samplingAllRadioButton, SIGNAL(toggled(bool)), this, SLOT(samplingAllToggle(bool)));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
	m_activeAvailable = true;
}

Post2dWindowArrowStructuredSettingDialog::~Post2dWindowArrowStructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowArrowStructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkStructuredGrid* g = dynamic_cast<vtkStructuredGrid*>(zoneData->data());
	g->GetDimensions(m_gridDims);
	setupSolutionComboBox(zoneData);
}

void Post2dWindowArrowStructuredSettingDialog::setSettings(const Post2dWindowNodeVectorArrowGroupDataItem::Setting& s, const Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting& sts)
{
	m_setting = s;
	m_stSetting = sts;

	// target
	ui->solutionComboBox->setCurrentText(s.arrowSetting.target);
	ui->scalarComboBox->setCurrentIndex(s.arrowSetting.colorTarget);
	ui->colorEditWidget->setColor(s.arrowSetting.customColor);

	// mapping
	switch (ArrowSettingContainer::ColorMode(s.arrowSetting.ColorMode)) {
	case ArrowSettingContainer::ColorMode::Custom:
		ui->specificRadioButton->setChecked(true);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}
	// standardValue
	ui->stdValueSpinBox->setValue(s.arrowSetting.standardValue);

	// legendLength
	ui->legendLengthSpinBox->setValue(s.legendLength);
	switch (setting.lengthMode()) {
	case ArrowSettingContainer::LengthMode::Auto:
		ui->lengthAutoCheckBox->setChecked(true);
		break;
	case ArrowSettingContainer::LengthMode::Custom:
		ui->lengthAutoCheckBox->setChecked(false);
		break;
	}
	ui->stdValueSpinBox->setValue(setting.standardValue());
	ui->legendLengthSpinBox->setValue(setting.legendLength());
	ui->minValueSpinBox->setValue(setting.minimumValue());

	// arrowSetting
	ui->arrowSizeSpinBox->setValue(s.arrowShape.arrowSize);
	ui->lineWidthSpinBox->setValue(s.arrowShape.lineWidth);

	// lengthMode
	if (s.arrowSetting.lengthMode == ArrowSettingContainer::LengthMode::Auto) {
		ui->lengthAutoCheckBox->setChecked(true);
	} else {
		ui->lengthAutoCheckBox->setChecked(false);
	}
}

ArrowSettingContainer Post2dWindowArrowStructuredSettingDialog::setting() const
{
	ArrowSettingContainer ret;
	ret.setAttribute(ui->solutionComboBox->currentText());
	ret.setColorAttribute(ui->scalarComboBox->currentText());
	ret.setCustomColor(ui->colorEditWidget->color());
	if (ui->specificRadioButton->isChecked()) {
		ret.setColorMode(ArrowSettingContainer::ColorMode::Custom);
	} else if (ui->scalarRadioButton->isChecked()) {
		ret.setColorMode(ArrowSettingContainer::ColorMode::ByScalar);
	}
	if (ui->lengthAutoCheckBox->isChecked()) {
		ret.setLengthMode(ArrowSettingContainer::LengthMode::Auto);
	} else {
		ret.setLengthMode(ArrowSettingContainer::LengthMode::Custom);
	}
	ret.setStandardValue(ui->stdValueSpinBox->value());
	ret.setLegendLength(ui->legendLengthSpinBox->value());
	ret.setMinimumValue(ui->minValueSpinBox->value());

	// minimumValue
	return ui->minValueSpinBox->setValue(s.minimumValue);

	// samplingRates
	ui->iSamplingRateSpinBox->setValue(sts.iSampleRate);
	ui->jSamplingRateSpinBox->setValue(sts.jSampleRate);
	ui->samplingSkipRadioButton->setChecked(true);
	if (sts.iSampleRate == 1 && sts.jSampleRate == 1) {
		ui->samplingAllRadioButton->setChecked(true);
	}

	return ret;
}

Post2dWindowNodeVectorArrowGroupDataItem::Setting Post2dWindowArrowStructuredSettingDialog::setting() const
{
	Post2dWindowNodeVectorArrowGroupDataItem::Setting ret = m_setting;

	// currentSolution
	int index = ui->solutionComboBox->currentIndex();
	ret.target = m_targets.at(index).c_str();

	// scalarValue
	index = ui->scalarComboBox->currentIndex();
	ret.scalarValueName = m_scalars.at(index).c_str();

	// color
	ret.color = ui->colorEditWidget->color();

	// mapping
	if (ui->specificRadioButton->isChecked()) {ret.mapping = Post2dWindowNodeVectorArrowGroupDataItem::Specific;}
	if (ui->scalarRadioButton->isChecked()) {ret.mapping = Post2dWindowNodeVectorArrowGroupDataItem::Scalar;}

	// arrowSetting
	ret.arrowSetting.setArrowSize(ui->arrowSizeSpinBox->value());
	ret.arrowSetting.setLineWidth(ui->lineWidthSpinBox->value());

	// lengthMode
	if (ui->lengthAutoCheckBox->isChecked()) {
		ret.lengthMode = Post2dWindowNodeVectorArrowGroupDataItem::lenAuto;
	} else {
		ret.lengthMode = Post2dWindowNodeVectorArrowGroupDataItem::lenCustom;
	}

	// standardValue
	ret.standardValue = ui->stdValueSpinBox->value();

	// legendLength;
	ret.legendLength = ui->legendLengthSpinBox->value();

	// minimumValue
	ret.minimumValue = ui->minValueSpinBox->value();

	return ret;
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting Post2dWindowArrowStructuredSettingDialog::stSetting() const
{
	Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting ret = m_stSetting;

	// samplingRates
	ret.iSampleRate = ui->iSamplingRateSpinBox->value();
	ret.jSampleRate = ui->jSamplingRateSpinBox->value();

	return ret;
}

void Post2dWindowArrowStructuredSettingDialog::setupSolutionComboBox(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_targets), ui->solutionComboBox);

	m_scalars = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_scalars), ui->scalarComboBox);

	if (m_targets.size() < 2) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post2dWindowArrowStructuredSettingDialog::samplingAllToggle(bool toggled)
{
	if (! toggled) {return;}
	ui->iSamplingRateSpinBox->setValue(1);
	ui->jSamplingRateSpinBox->setValue(1);
}

void Post2dWindowArrowStructuredSettingDialog::showRegionDialog()
{
	Post2dGridRegionSelectDialog dialog(this);
	if (! m_activeAvailable) {
		dialog.disableActive();
	}
	dialog.setGridSize(m_gridDims[0], m_gridDims[1]);
	dialog.setRegionMode(m_setting.regionMode);
	dialog.setRegion(m_stSetting.range);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.regionMode = dialog.regionMode();
	m_stSetting.range = dialog.region();
}

void Post2dWindowArrowStructuredSettingDialog::disableActive()
{
	m_activeAvailable = false;
}
