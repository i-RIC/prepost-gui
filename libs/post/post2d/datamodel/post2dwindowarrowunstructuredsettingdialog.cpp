#include "ui_post2dwindowarrowunstructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowarrowunstructuredsettingdialog.h"

#include <guibase/comboboxtool.h>
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

void Post2dWindowArrowUnstructuredSettingDialog::setSettings(const Post2dWindowNodeVectorArrowGroupDataItem::Setting& s, const Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting& unss)
{
	ui->solutionComboBox->setCurrentText(s.arrowSetting.target);
	ui->scalarComboBox->setCurrentText(s.arrowSetting.colorTarget);
	ui->colorEditWidget->setColor(s.arrowSetting.customColor);

	switch (setting.arrowSetting.samplingMode.value()) {
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

	ui->samplingRateSpinBox->setValue(setting.arrowSetting.samplingRate);
	ui->samplingNumberSpinBox->setValue(setting.arrowSetting.samplingNumber);

	switch (setting.arrowSetting.colorMode.value())) {
	case ArrowSettingContainer::ColorMode::Custom:
		ui->specificRadioButton->setChecked(true);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}

	ui->stdValueSpinBox->setValue(setting.arrowSetting.standardValue);
	ui->legendLengthSpinBox->setValue(setting.arrowSetting.legendLength);
	ui->minValueSpinBox->setValue(setting.arrowSetting.minimumValue);
}

Post2dWindowNodeVectorArrowGroupDataItem::Setting Post2dWindowArrowUnstructuredSettingDialog::setting() const
{
	Post2dWindowNodeVectorArrowGroupDataItem::Setting ret = m_setting;

	ret.arrowSetting.target = ui->solutionComboBox->currentText();
	ret.arrowSetting.colorTarget = ui->scalarComboBox->currentText();
	ret.arrowSetting.customColor = ui->colorEditWidget->color();

	if (ui->samplingAllRadioButton->isChecked()) {
		ret.arrowSetting.samplingMode = ArrowSettingContainer::SamplingMode::All;
	} else if (ui->samplingRateRadioButton->isChecked()) {
		ret.arrowSetting.samplingMode = ArrowSettingContainer::SamplingMode::Rate;
	}	else {
		ret.arrowSetting.samplingMode = ArrowSettingContainer::SamplingMode::Number;
	}

	ret.arrowSetting.samplingRate = ui->samplingRateSpinBox->value();
	ret.arrowSetting.samplingNumber = ui->samplingNumberSpinBox->value();

	if (ui->specificRadioButton->isChecked()) {
		ret.arrowSetting.colorMode = ArrowSettingContainer::ColorMode::Custom;
	} else if (ui->scalarRadioButton->isChecked()) {
		ret.arrowSetting.colorMode = ArrowSettingContainer::ColorMode::ByScalar;
	}

	if (ui->lengthAutoCheckBox->isChecked()) {
		ret.arrowSetting.lengthMode = ArrowSettingContainer::LengthMode::Auto;
	} else {
		ret.arrowSetting.lengthMode = ArrowSettingContainer::LengthMode::Custom;
	}

	ret.arrowSetting.standardValue = ui->stdValueSpinBox->value();
	ret.arrowSetting.legendLength = ui->legendLengthSpinBox->value();
	ret.arrowSetting.minimumValue = ui->minValueSpinBox->value();

	return ret;
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
