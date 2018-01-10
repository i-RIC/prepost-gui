#include "ui_post2dwindowarrowunstructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowarrowunstructuredsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/scalarbardialog.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <QtGlobal>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowArrowUnstructuredSettingDialog::Post2dWindowArrowUnstructuredSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowArrowUnstructuredSettingDialog)
{
	ui->setupUi(this);

	connect(ui->specificRadioButton, SIGNAL(toggled(bool)), ui->colorEditWidget, SLOT(setEnabled(bool)));
	connect(ui->scalarRadioButton, SIGNAL(toggled(bool)), ui->scalarComboBox, SLOT(setEnabled(bool)));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showScalarBarDialog()));
}

Post2dWindowArrowUnstructuredSettingDialog::~Post2dWindowArrowUnstructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowArrowUnstructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkPointSet* p = zoneData->data();
	ui->samplingNumberSpinBox->setMaximum(p->GetNumberOfPoints());
	setupSolutionComboBox(zoneData);
}

void Post2dWindowArrowUnstructuredSettingDialog::disableActive()
{
	m_activeAvailable = false;
}

Post2dWindowNodeVectorArrowUnstructuredSetting Post2dWindowArrowUnstructuredSettingDialog::setting() const
{
	Post2dWindowNodeVectorArrowUnstructuredSetting ret = m_setting;

	ret.target = ui->solutionComboBox->currentText();

	if (ui->samplingAllRadioButton->isChecked()) {
		ret.samplingMode = ArrowSettingContainer::SamplingMode::All;
	} else if (ui->samplingRateRadioButton->isChecked()) {
		ret.samplingMode = ArrowSettingContainer::SamplingMode::Rate;
	}	else {
		ret.samplingMode = ArrowSettingContainer::SamplingMode::Number;
	}
	ret.samplingRate = ui->samplingRateSpinBox->value();
	ret.samplingNumber = ui->samplingNumberSpinBox->value();

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

	return ret;
}

QString Post2dWindowArrowUnstructuredSettingDialog::scalarBarTitle() const
{
		return m_colorBarTitleMap[colorScalar()];
}

void Post2dWindowArrowUnstructuredSettingDialog::setSetting(const Post2dWindowNodeVectorArrowUnstructuredSetting& setting)
{
	m_setting = setting;

	ui->solutionComboBox->setCurrentText(setting.target);

	switch (setting.samplingMode.value()) {
	case ArrowSettingContainer::SamplingMode::All:
		ui->samplingAllRadioButton->setChecked(true);
		break;
	case ArrowSettingContainer::SamplingMode::Rate:
		ui->samplingRateRadioButton->setChecked(true);
		break;
	case ArrowSettingContainer::SamplingMode::Number:
		ui->samplingNumberRadioButton->setChecked(true);
		break;
	}
	ui->samplingRateSpinBox->setValue(setting.samplingRate);
	ui->samplingNumberSpinBox->setValue(setting.samplingNumber);

	switch (setting.colorMode.value()) {
	case ArrowSettingContainer::ColorMode::Custom:
		ui->specificRadioButton->setChecked(true);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}
	ui->colorEditWidget->setColor(setting.customColor);
	ui->scalarComboBox->setCurrentText(setting.colorTarget);

	switch (setting.lengthMode.value()) {
	case ArrowSettingContainer::LengthMode::Auto:
		ui->lengthAutoCheckBox->setChecked(true);
		break;
	case ArrowSettingContainer::LengthMode::Custom:
		ui->lengthAutoCheckBox->setChecked(false);
		break;
	}
	ui->stdValueSpinBox->setValue(setting.standardValue);
	ui->legendLengthSpinBox->setValue(setting.legendLength);
	ui->minValueSpinBox->setValue(setting.minimumValue);

	ui->arrowSizeSpinBox->setValue(setting.arrowSize);
	ui->lineWidthSpinBox->setValue(setting.lineWidth);
}

void Post2dWindowArrowUnstructuredSettingDialog::setColorBarTitleMap(const QMap<std::string, QString>& titlemap)
{
	m_colorBarTitleMap = titlemap;
}

void Post2dWindowArrowUnstructuredSettingDialog::showRegionDialog()
{
	Post2dGridRegionSelectDialog dialog(this);
	if (! m_activeAvailable) {
		dialog.disableActive();
	}
	dialog.hideCustom();
	dialog.setRegionMode(m_setting.regionMode);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.regionMode = dialog.regionMode();
}

void Post2dWindowArrowUnstructuredSettingDialog::showScalarBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setSetting(m_setting.scalarBarSetting);
	dialog.setTitle(m_colorBarTitleMap[colorScalar()]);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.scalarBarSetting = dialog.setting();
	m_colorBarTitleMap[colorScalar()] = dialog.title();
}

void Post2dWindowArrowUnstructuredSettingDialog::setupSolutionComboBox(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();

	m_solutions = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_solutions), ui->solutionComboBox);

	m_scalars = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_scalars), ui->scalarComboBox);

	if (m_solutions.size() < 2) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

std::string Post2dWindowArrowUnstructuredSettingDialog::colorScalar() const
{
	return m_scalars.at(ui->scalarComboBox->currentIndex());
}
