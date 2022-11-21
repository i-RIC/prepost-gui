#include "ui_post2dwindowarrowstructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowarrowstructuredsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/scalarbardialog.h>
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
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showScalarBarDialog()));
	m_activeDisabled = false;
}

Post2dWindowArrowStructuredSettingDialog::~Post2dWindowArrowStructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowArrowStructuredSettingDialog::setGridType(Post2dWindowGridTypeDataItem* item)
{
	m_gridTypeDataItem = item;
}

void Post2dWindowArrowStructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkStructuredGrid* g = dynamic_cast<vtkStructuredGrid*>(zoneData->data());
	g->GetDimensions(m_gridDims);
	setupSolutionComboBox(zoneData);
}

void Post2dWindowArrowStructuredSettingDialog::disableActive()
{
	m_activeDisabled = true;
}

Post2dWindowNodeVectorArrowStructuredSetting Post2dWindowArrowStructuredSettingDialog::setting() const
{
	Post2dWindowNodeVectorArrowStructuredSetting ret = m_setting;

	ret.target = ui->solutionComboBox->currentText();

	// sampling mode setting not used

	if (ui->specificRadioButton->isChecked()) {
		ret.colorMode = ArrowSettingContainer::ColorMode::Custom;
	} else if (ui->scalarRadioButton->isChecked()) {
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

	ret.arrowSize = ui->arrowSizeSpinBox->value();
	ret.lineWidth = ui->lineWidthSpinBox->value();

	ret.iSampleRate = ui->iSamplingRateSpinBox->value();
	ret.jSampleRate = ui->jSamplingRateSpinBox->value();

	ret.legendTextSetting = ui->legendTextWidget->setting();

	return ret;
}

QString Post2dWindowArrowStructuredSettingDialog::scalarBarTitle() const
{
		return m_colorBarTitleMap[colorScalar()];
}

void Post2dWindowArrowStructuredSettingDialog::setSetting(const Post2dWindowNodeVectorArrowStructuredSetting& s)
{
	m_setting = s;

	ui->solutionComboBox->setCurrentText(s.target);

	if (s.iSampleRate == 1 && s.jSampleRate == 1) {
		ui->samplingAllRadioButton->setChecked(true);
	} else {
		ui->samplingSkipRadioButton->setChecked(true);
	}
	// sampling mode setting not used

	ui->colorEditWidget->setColor(s.customColor);
	ui->scalarComboBox->setCurrentText(s.colorTarget);

	switch (s.lengthMode.value()) {
	case ArrowSettingContainer::LengthMode::Auto:
		ui->lengthAutoCheckBox->setChecked(true);
		break;
	case ArrowSettingContainer::LengthMode::Custom:
		ui->lengthAutoCheckBox->setChecked(false);
		break;
	}
	ui->stdValueSpinBox->setValue(s.standardValue);
	ui->legendLengthSpinBox->setValue(s.legendLength);
	ui->minValueSpinBox->setValue(s.minimumValue);

	ui->arrowSizeSpinBox->setValue(s.arrowSize);
	ui->lineWidthSpinBox->setValue(s.lineWidth);

	switch (s.colorMode.value()) {
	case ArrowSettingContainer::ColorMode::Custom:
		ui->specificRadioButton->setChecked(true);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}
	ui->iSamplingRateSpinBox->setValue(s.iSampleRate);
	ui->jSamplingRateSpinBox->setValue(s.jSampleRate);

	ui->legendTextWidget->setSetting(s.legendTextSetting);
}

void Post2dWindowArrowStructuredSettingDialog::setColorBarTitleMap(const QMap<std::string, QString>& titlemap)
{
	m_colorBarTitleMap = titlemap;
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

void Post2dWindowArrowStructuredSettingDialog::showRegionDialog()
{
	Post2dGridRegionSelectDialog dialog(this);
	if (m_activeDisabled) {
		dialog.disableActive();
	}
	dialog.setGridSize(m_gridDims[0], m_gridDims[1]);
	dialog.setRegionMode(m_setting.regionMode);
	dialog.setRegion(m_setting.range);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.regionMode = dialog.regionMode();
	m_setting.range = dialog.region();
}

void Post2dWindowArrowStructuredSettingDialog::showScalarBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setSetting(m_setting.scalarBarSetting);
	dialog.setTitle(m_colorBarTitleMap[colorScalar()]);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.scalarBarSetting = dialog.setting();
	m_colorBarTitleMap[colorScalar()] = dialog.title();
}

void Post2dWindowArrowStructuredSettingDialog::samplingAllToggle(bool toggled)
{
	if (! toggled) {return;}
	ui->iSamplingRateSpinBox->setValue(1);
	ui->jSamplingRateSpinBox->setValue(1);
}

std::string Post2dWindowArrowStructuredSettingDialog::colorScalar() const
{
	return m_scalars.at(ui->scalarComboBox->currentIndex());
}
