#include "postparticlevectorpropertydialog.h"
#include "ui_postparticlevectorpropertydialog.h"
#include "postwindowgridtypedataiteminterface.h"

#include <vtkPolyData.h>
#include <vtkPointData.h>

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/arrowshapecontainer.h>

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

void PostParticleVectorPropertyDialog::setZoneData(PostZoneDataContainer* data)
{
	ui->samplingNumberSpinBox->setMaximum(data->particleData()->GetNumberOfPoints());
	setupComboBoxes(data);
}

ArrowSettingContainer PostParticleVectorPropertyDialog::arrowSetting() const
{
	ArrowSettingContainer c;
	c.setAttribute(ui->attributeComboBox->currentText());

	if (ui->samplingAllRadioButton->isChecked()) {
		c.setSamplingMode(ArrowSettingContainer::SamplingMode::All);
	} else if (ui->samplingRateRadioButton->isChecked()) {
		c.setSamplingMode(ArrowSettingContainer::SamplingMode::Rate);
	} else if (ui->samplingNumberRadioButton->isChecked()) {
		c.setSamplingMode(ArrowSettingContainer::SamplingMode::Number);
	}
	c.setSamplingRate(ui->samplingRateSpinBox->value());
	c.setSamplingNumber(ui->samplingNumberSpinBox->value());

	if (ui->specificRadioButton->isChecked()) {
		c.setColorMode(ArrowSettingContainer::ColorMode::Custom);
	} else if (ui->scalarRadioButton->isChecked()) {
		c.setColorMode(ArrowSettingContainer::ColorMode::ByScalar);
	}
	c.setCustomColor(ui->colorEditWidget->color());
	c.setColorAttribute(ui->scalarComboBox->currentText());

	if (ui->lengthAutoCheckBox->isChecked()) {
		c.setLengthMode(ArrowSettingContainer::LengthMode::Auto);
	} else {
		c.setLengthMode(ArrowSettingContainer::LengthMode::Custom);
	}
	c.setStandardValue(ui->stdValueSpinBox->value());
	c.setLegendLength(ui->legendLengthSpinBox->value());
	c.setMinimumValue(ui->minValueSpinBox->value());

	return c;
}

void PostParticleVectorPropertyDialog::setArrowSetting(const ArrowSettingContainer& s)
{
	ui->attributeComboBox->setCurrentText(s.attribute());

	switch (s.samplingMode()) {
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
	ui->samplingRateSpinBox->setValue(s.samplingRate());
	ui->samplingNumberSpinBox->setValue(s.samplingNumber());

	switch (s.colorMode()) {
	case ArrowSettingContainer::ColorMode::Custom:
		ui->specificRadioButton->setChecked(true);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}
	ui->colorEditWidget->setColor(s.customColor());
	ui->scalarComboBox->setCurrentText(s.colorAttribute());

	ui->lengthAutoCheckBox->setChecked(s.lengthMode() == ArrowSettingContainer::LengthMode::Auto);
	ui->stdValueSpinBox->setValue(s.standardValue());
	ui->legendLengthSpinBox->setValue(s.legendLength());
	ui->minValueSpinBox->setValue(s.minimumValue());
}

ArrowShapeContainer PostParticleVectorPropertyDialog::arrowShape() const
{
	ArrowShapeContainer cont;
	cont.setArrowSize(ui->arrowSizeSpinBox->value());
	cont.setLineWidth(ui->lineWidthSpinBox->value());
	return cont;
}

void PostParticleVectorPropertyDialog::setArrowShape(const ArrowShapeContainer& arrowShape)
{
	ui->arrowSizeSpinBox->setValue(arrowShape.arrowSize());
	ui->lineWidthSpinBox->setValue(arrowShape.lineWidth());
}

void PostParticleVectorPropertyDialog::setupComboBoxes(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->particleData()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();
	int num = pd->GetNumberOfArrays();
	ui->attributeComboBox->blockSignals(true);
	ui->scalarComboBox->blockSignals(true);
	for (int i = 0; i < num; ++i) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {continue;}
		QString name = tmparray->GetName();
		if (tmparray->GetNumberOfComponents() == 1) {
			// scalar attributes.
			ui->scalarComboBox->addItem(gt->solutionCaption(name));
			m_scalars.append(name);
		} else {
			// vector attributes.
			ui->attributeComboBox->addItem(gt->solutionCaption(name));
			m_attributes.append(name);
		}
	}
	ui->attributeComboBox->blockSignals(false);
	ui->scalarComboBox->blockSignals(false);
	if (m_attributes.count() <= 1) {
		ui->attributeLabel->hide();
		ui->attributeComboBox->hide();
	}
	if (m_scalars.size() == 0) {
		ui->scalarRadioButton->hide();
		ui->scalarComboBox->hide();
	}
}
