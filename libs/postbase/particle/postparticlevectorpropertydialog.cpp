#include "postparticlevectorpropertydialog.h"
#include "postwindowgridtypedataiteminterface.h"
#include "ui_postparticlevectorpropertydialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>

#include <vtkPolyData.h>
#include <vtkPointData.h>


PostParticleVectorPropertyDialog::PostParticleVectorPropertyDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PostParticleVectorPropertyDialog)
{
	ui->setupUi(this);
}

PostParticleVectorPropertyDialog::~PostParticleVectorPropertyDialog()
{
	delete ui;
}

void PostParticleVectorPropertyDialog::setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item)
{
	m_gridTypeDataItem = item;
}

void PostParticleVectorPropertyDialog::setData(vtkPolyData* data)
{
	ui->samplingNumberSpinBox->setMaximum(data->GetNumberOfPoints());
	setupComboBoxes(data);
}

ArrowSettingContainer PostParticleVectorPropertyDialog::setting() const
{
	ArrowSettingContainer c;

	c.target = ui->targetComboBox->currentText();

	if (ui->samplingAllRadioButton->isChecked()) {
		c.samplingMode = ArrowSettingContainer::SamplingMode::All;
	} else if (ui->samplingRateRadioButton->isChecked()) {
		c.samplingMode = ArrowSettingContainer::SamplingMode::Rate;
	} else if (ui->samplingNumberRadioButton->isChecked()) {
		c.samplingMode = ArrowSettingContainer::SamplingMode::Number;
	}

	c.samplingRate = ui->samplingRateSpinBox->value();
	c.samplingNumber = ui->samplingNumberSpinBox->value();

	if (ui->specificRadioButton->isChecked()) {
		c.colorMode = ArrowSettingContainer::ColorMode::Custom;
	} else if (ui->scalarRadioButton->isChecked()) {
		c.colorMode = ArrowSettingContainer::ColorMode::ByScalar;
	}

	c.customColor = ui->colorEditWidget->color();
	c.colorTarget = ui->colorTargetComboBox->currentText();

	if (ui->lengthAutoCheckBox->isChecked()) {
		c.lengthMode = ArrowSettingContainer::LengthMode::Auto;
	} else {
		c.lengthMode = ArrowSettingContainer::LengthMode::Custom;
	}
	c.standardValue = ui->stdValueSpinBox->value();
	c.legendLength = ui->legendLengthSpinBox->value();
	c.minimumValue = ui->minValueSpinBox->value();

	c.arrowSize = ui->arrowSizeSpinBox->value();
	c.lineWidth = ui->lineWidthSpinBox->value();

	return c;
}

void PostParticleVectorPropertyDialog::setSetting(const ArrowSettingContainer& s)
{
	ui->targetComboBox->setCurrentText(s.target);

	switch (s.samplingMode.value()) {
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
	ui->samplingRateSpinBox->setValue(s.samplingRate);
	ui->samplingNumberSpinBox->setValue(s.samplingNumber);

	switch (s.colorMode.value()) {
	case ArrowSettingContainer::ColorMode::Custom:
		ui->specificRadioButton->setChecked(true);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}
	ui->colorEditWidget->setColor(s.customColor);
	ui->colorTargetComboBox->setCurrentText(s.colorTarget);

	ui->lengthAutoCheckBox->setChecked(s.lengthMode == ArrowSettingContainer::LengthMode::Auto);
	ui->stdValueSpinBox->setValue(s.standardValue);
	ui->legendLengthSpinBox->setValue(s.legendLength);
	ui->minValueSpinBox->setValue(s.minimumValue);

	ui->arrowSizeSpinBox->setValue(s.arrowSize);
	ui->lineWidthSpinBox->setValue(s.lineWidth);
}

void PostParticleVectorPropertyDialog::setupComboBoxes(vtkPolyData* data)
{
	auto pd = data->GetPointData();
	auto gt = m_gridTypeDataItem->gridType();

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_targets), ui->targetComboBox);

	m_colorTargets = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_colorTargets), ui->colorTargetComboBox);
	if (m_targets.size() < 2) {
		ui->attributeLabel->hide();
		ui->targetComboBox->hide();
	}
	if (m_colorTargets.size() == 0) {
		ui->scalarRadioButton->hide();
		ui->colorTargetComboBox->hide();
	}
}
