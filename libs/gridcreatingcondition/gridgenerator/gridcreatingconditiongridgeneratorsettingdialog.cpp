#include "ui_gridcreatingconditiongridgeneratorsettingdialog.h"

#include "gridcreatingconditiongridgeneratorsettingdialog.h"

#include <QPushButton>

GridCreatingConditionGridGeneratorSettingDialog::GridCreatingConditionGridGeneratorSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionGridGeneratorSettingDialog)
{
	ui->setupUi(this);
	ui->singleSectionRadioButton->setChecked(true);

	ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("&Create Grid"));
}

GridCreatingConditionGridGeneratorSettingDialog::~GridCreatingConditionGridGeneratorSettingDialog()
{
	delete ui;
}

void GridCreatingConditionGridGeneratorSettingDialog::setReadOnly(bool readOnly)
{
	ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(readOnly);
}

void GridCreatingConditionGridGeneratorSettingDialog::setChannelShape(ChannelShape c)
{
	switch (c) {
	case SingleSection:
		ui->singleSectionRadioButton->setChecked(true);
		break;
	case SimpleCompound:
		ui->simpleCompoundRadioButton->setChecked(true);
		break;
	case ComplexCompound:
		ui->complexCompoundRadioButton->setChecked(true);
		break;
	}
}

void GridCreatingConditionGridGeneratorSettingDialog::setFloodChannelProperty(FloodChannel f)
{
	switch (f) {
	case Fixed:
		ui->fixedRadioButton->setChecked(true);
		break;
	case Movable:
		ui->movableRadioButton->setChecked(true);
		break;
	}
}

void GridCreatingConditionGridGeneratorSettingDialog::setBedShape(BedShape b)
{
	switch (b) {
	case Flat:
		ui->flatRadioButton->setChecked(true);
		break;
	case AlternateBar:
		ui->barRadioButton->setChecked(true);
		break;
	case Parabolic:
		ui->parabolicRadioButton->setChecked(true);
		break;
	}
}

void GridCreatingConditionGridGeneratorSettingDialog::setAmplitude(double amp)
{
	ui->amplitudeSpinBox->setValue(amp);
}

void GridCreatingConditionGridGeneratorSettingDialog::setLag(double lag)
{
	ui->lagSpinBox->setValue(lag);
}

void GridCreatingConditionGridGeneratorSettingDialog::setWaveLength(double length)
{
	ui->waveLengthSpinBox->setValue(length);
}

void GridCreatingConditionGridGeneratorSettingDialog::setWaveNumber(int waveNum)
{
	ui->waveNumberSpinBox->setValue(waveNum);
}

void GridCreatingConditionGridGeneratorSettingDialog::setAngle(double angle)
{
	ui->meanderAngleSpinBox->setValue(angle);
}

void GridCreatingConditionGridGeneratorSettingDialog::setSlope(double slope)
{
	ui->slopeSpinBox->setValue(slope);
}

void GridCreatingConditionGridGeneratorSettingDialog::setLongitudinalGridNumber(int gridNum)
{
	ui->longitudinalGridNumberSpinBox->setValue(gridNum);
}

void GridCreatingConditionGridGeneratorSettingDialog::setSimpleChannelWidth(double width)
{
	ui->simpleChannelWidthSpinBox->setValue(width);
}

void GridCreatingConditionGridGeneratorSettingDialog::setSimpleChannelGridNumber(int gridNum)
{
	ui->simpleChannelGridNumberSpinBox->setValue(gridNum);
}

void GridCreatingConditionGridGeneratorSettingDialog::setSimpleCompoundLeftWidth(double width)
{
	ui->leftWidthSpinBox->setValue(width);
}

void GridCreatingConditionGridGeneratorSettingDialog::setSimpleCompoundRightWidth(double width)
{
	ui->rightWidthSpinBox->setValue(width);
}

void GridCreatingConditionGridGeneratorSettingDialog::setSimpleCompoundLowWidth(double width)
{
	ui->simpleLowWidthSpinBox->setValue(width);
}

void GridCreatingConditionGridGeneratorSettingDialog::setComplexCompoundTotalWidth(double width)
{
	ui->totalWidthSpinBox->setValue(width);
}

void GridCreatingConditionGridGeneratorSettingDialog::setComplexCompoundLowWidth(double width)
{
	ui->complexLowWidthSpinBox->setValue(width);
}

void GridCreatingConditionGridGeneratorSettingDialog::setLeftGridNumber(int gridNum)
{
	ui->leftGridNumberSpinBox->setValue(gridNum);
}

void GridCreatingConditionGridGeneratorSettingDialog::setRightGridNumber(int gridNum)
{
	ui->rightGridNumberSpinBox->setValue(gridNum);
}

void GridCreatingConditionGridGeneratorSettingDialog::setLowGridNumber(int gridNum)
{
	ui->lowGridNumberSpinBox->setValue(gridNum);
}

void GridCreatingConditionGridGeneratorSettingDialog::setLowDepth(double depth)
{
	ui->depthSpinBox->setValue(depth);
}

void GridCreatingConditionGridGeneratorSettingDialog::setLowSlope(double slope)
{
	ui->lowSlopeSpinBox->setValue(slope);
}

void GridCreatingConditionGridGeneratorSettingDialog::setBankGridNumber(int gridNum)
{
	ui->bankGridNumberSpinBox->setValue(gridNum);
}

void GridCreatingConditionGridGeneratorSettingDialog::setWidthChange(WidthChange wc)
{
	switch (wc) {
	case Constant:
		ui->constantRadioButton->setChecked(true);
		break;
	case Change:
		ui->changeRadioButton->setChecked(true);
		break;
	}
}

void GridCreatingConditionGridGeneratorSettingDialog::setWidthChangeType(WidthChangeType wct)
{
	switch (wct) {
	case Both:
		ui->bothRadioButton->setChecked(true);
		break;
	case Left:
		ui->leftRadioButton->setChecked(true);
		break;
	case Right:
		ui->rightRadioButton->setChecked(true);
		break;
	}
}

void GridCreatingConditionGridGeneratorSettingDialog::setWidthVariation(double var)
{
	ui->variationSpinBox->setValue(var);
}

GridCreatingConditionGridGeneratorSettingDialog::ChannelShape GridCreatingConditionGridGeneratorSettingDialog::channelShape()
{
	if (ui->singleSectionRadioButton->isChecked()) { return SingleSection; }
	if (ui->simpleCompoundRadioButton->isChecked()) { return SimpleCompound; }
	if (ui->complexCompoundRadioButton->isChecked()) { return ComplexCompound; }
	// default
	return SingleSection;
}

GridCreatingConditionGridGeneratorSettingDialog::FloodChannel GridCreatingConditionGridGeneratorSettingDialog::floodChannel()
{
	if (ui->fixedRadioButton->isChecked()) { return Fixed; }
	if (ui->movableRadioButton->isChecked()) { return Movable; }
	// default
	return Fixed;
}

GridCreatingConditionGridGeneratorSettingDialog::BedShape GridCreatingConditionGridGeneratorSettingDialog::bedShape()
{
	if (ui->flatRadioButton->isChecked()) { return Flat; }
	if (ui->barRadioButton->isChecked()) { return AlternateBar; }
	if (ui->parabolicRadioButton->isChecked()) { return Parabolic; }
	// default
	return Flat;
}

double GridCreatingConditionGridGeneratorSettingDialog::amplitude()
{
	return ui->amplitudeSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::lag()
{
	return ui->lagSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::waveLength()
{
	return ui->waveLengthSpinBox->value();
}

int GridCreatingConditionGridGeneratorSettingDialog::waveNumber()
{
	return ui->waveNumberSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::angle()
{
	return ui->meanderAngleSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::slope()
{
	return ui->slopeSpinBox->value();
}

int GridCreatingConditionGridGeneratorSettingDialog::longitudinalGridNumber()
{
	return ui->longitudinalGridNumberSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::simpleChannelWidth()
{
	return ui->simpleChannelWidthSpinBox->value();
}

int GridCreatingConditionGridGeneratorSettingDialog::simpleChannelGridNumber()
{
	return ui->simpleChannelGridNumberSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::simpleCompoundLeftWidth()
{
	return ui->leftWidthSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::simpleCompoundRightWidth()
{
	return ui->rightWidthSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::simpleCompoundLowWidth()
{
	return ui->simpleLowWidthSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::complexCompoundTotalWidth()
{
	return ui->totalWidthSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::complexCompoundLowWidth()
{
	return ui->complexLowWidthSpinBox->value();
}

int GridCreatingConditionGridGeneratorSettingDialog::leftGridNumber()
{
	return ui->leftGridNumberSpinBox->value();
}

int GridCreatingConditionGridGeneratorSettingDialog::rightGridNumber()
{
	return ui->rightGridNumberSpinBox->value();
}

int GridCreatingConditionGridGeneratorSettingDialog::lowGridNumber()
{
	return ui->lowGridNumberSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::lowDepth()
{
	return ui->depthSpinBox->value();
}

double GridCreatingConditionGridGeneratorSettingDialog::lowSlope()
{
	return ui->lowSlopeSpinBox->value();
}

int GridCreatingConditionGridGeneratorSettingDialog::bankGridNumber()
{
	return ui->bankGridNumberSpinBox->value();
}

GridCreatingConditionGridGeneratorSettingDialog::WidthChange GridCreatingConditionGridGeneratorSettingDialog::widthChange()
{
	if (ui->constantRadioButton->isChecked()) { return Constant; }
	if (ui->changeRadioButton->isChecked()) { return Change; }
	// default
	return Constant;
}

GridCreatingConditionGridGeneratorSettingDialog::WidthChangeType GridCreatingConditionGridGeneratorSettingDialog::widthChangeType()
{
	if (ui->bothRadioButton->isChecked()) { return Both; }
	if (ui->leftRadioButton->isChecked()) { return Left; }
	if (ui->rightRadioButton->isChecked()) { return Right; }
	// default
	return Both;
}

double GridCreatingConditionGridGeneratorSettingDialog::widthVariation()
{
	return ui->variationSpinBox->value();
}

void GridCreatingConditionGridGeneratorSettingDialog::setupConnections()
{
	connect(ui->singleSectionRadioButton, SIGNAL(toggled(bool)), ui->simpleCompoundGroupBox, SLOT(setDisabled(bool)));
	connect(ui->singleSectionRadioButton, SIGNAL(toggled(bool)), ui->complexCompoundGroupBox, SLOT(setDisabled(bool)));
	connect(ui->simpleCompoundRadioButton, SIGNAL(toggled(bool)), ui->simpleChannelGroupBox, SLOT(setDisabled(bool)));
	connect(ui->simpleCompoundRadioButton, SIGNAL(toggled(bool)), ui->complexCompoundGroupBox, SLOT(setDisabled(bool)));
	connect(ui->complexCompoundRadioButton, SIGNAL(toggled(bool)), ui->simpleChannelGroupBox, SLOT(setDisabled(bool)));
	connect(ui->complexCompoundRadioButton, SIGNAL(toggled(bool)), ui->simpleCompoundGroupBox, SLOT(setDisabled(bool)));

	connect(ui->singleSectionRadioButton, SIGNAL(toggled(bool)), ui->compoundTab, SLOT(setDisabled(bool)));
	connect(ui->simpleCompoundRadioButton, SIGNAL(toggled(bool)), ui->widthTab, SLOT(setDisabled(bool)));
	connect(ui->complexCompoundRadioButton, SIGNAL(toggled(bool)), ui->widthTab, SLOT(setDisabled(bool)));

	connect(ui->flatRadioButton, SIGNAL(toggled(bool)), ui->amplitudeGroupBox, SLOT(setDisabled(bool)));
	connect(ui->flatRadioButton, SIGNAL(toggled(bool)), ui->lagGroupBox, SLOT(setDisabled(bool)));
	connect(ui->parabolicRadioButton, SIGNAL(toggled(bool)), ui->lagGroupBox, SLOT(setDisabled(bool)));

	connect(ui->constantRadioButton, SIGNAL(toggled(bool)), ui->widthChangeTypeGroupBox, SLOT(setDisabled(bool)));
	connect(ui->constantRadioButton, SIGNAL(toggled(bool)), ui->widthVariationLabel, SLOT(setDisabled(bool)));
	connect(ui->constantRadioButton, SIGNAL(toggled(bool)), ui->variationSpinBox, SLOT(setDisabled(bool)));
}

void GridCreatingConditionGridGeneratorSettingDialog::setupDisability()
{
	ui->amplitudeGroupBox->setEnabled(ui->barRadioButton->isChecked() || ui->parabolicRadioButton->isChecked());
	ui->lagGroupBox->setEnabled(ui->barRadioButton->isChecked());

	ui->simpleChannelGroupBox->setEnabled(ui->singleSectionRadioButton->isChecked());
	ui->simpleCompoundGroupBox->setEnabled(ui->simpleCompoundRadioButton->isChecked());
	ui->complexCompoundGroupBox->setEnabled(ui->complexCompoundRadioButton->isChecked());

	ui->compoundTab->setEnabled(ui->simpleCompoundRadioButton->isChecked() || ui->complexCompoundRadioButton->isChecked());
	ui->widthTab->setEnabled(ui->singleSectionRadioButton->isChecked());

	ui->widthChangeTypeGroupBox->setEnabled(ui->singleSectionRadioButton->isChecked() && ui->changeRadioButton->isChecked());
	ui->widthVariationLabel->setEnabled(ui->singleSectionRadioButton->isChecked() && ui->changeRadioButton->isChecked());
	ui->variationSpinBox->setEnabled(ui->singleSectionRadioButton->isChecked() && ui->changeRadioButton->isChecked());
}
