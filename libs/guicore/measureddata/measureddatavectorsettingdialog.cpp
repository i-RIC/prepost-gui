#include "ui_measureddatavectorsettingdialog.h"

#include "../project/measureddata.h"
#include "measureddatavectorsettingdialog.h"

#include <QtGlobal>

MeasuredDataVectorSettingDialog::MeasuredDataVectorSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::MeasuredDataVectorSettingDialog)
{
	ui->setupUi(this);
}

MeasuredDataVectorSettingDialog::~MeasuredDataVectorSettingDialog()
{
	delete ui;
}

void MeasuredDataVectorSettingDialog::setData(MeasuredData* data)
{
	setupSolutionComboBox(data);
}

void MeasuredDataVectorSettingDialog::setSolution(const QString& sol)
{
	int index = m_solutions.indexOf(sol);
	if (index == -1) {index = 0;}
	ui->solutionComboBox->setCurrentIndex(index);
}

QString MeasuredDataVectorSettingDialog::solution() const
{
	int index = ui->solutionComboBox->currentIndex();
	if (index < 0 || index > m_solutions.count() - 1) {return "";}
	return m_solutions.at(index);
}

void MeasuredDataVectorSettingDialog::setScalarValue(const QString& scalar)
{
	int index = m_scalars.indexOf(scalar);
	if (index == -1) { index = 0; }
	ui->scalarComboBox->setCurrentIndex(index);
}

QString MeasuredDataVectorSettingDialog::scalarValue() const
{
	int index = ui->scalarComboBox->currentIndex();
	if (index < 0 || index > m_scalars.count() - 1) {return "";}
	return m_scalars.at(index);
}

void MeasuredDataVectorSettingDialog::setColor(const QColor& color)
{
	ui->colorEditWidget->setColor(color);
}

QColor MeasuredDataVectorSettingDialog::color() const
{
	return ui->colorEditWidget->color();
}

void MeasuredDataVectorSettingDialog::setColorMode(MeasuredData::ArrowColorMode cm)
{
	switch (cm) {
	case MeasuredData::acmSpecific:
		ui->specificRadioButton->setChecked(true);
		break;
	case MeasuredData::acmScalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}
}

MeasuredData::ArrowColorMode MeasuredDataVectorSettingDialog::colorMode() const
{
	if (ui->specificRadioButton->isChecked()) {return MeasuredData::acmSpecific;}
	if (ui->scalarRadioButton->isChecked()) {return MeasuredData::acmScalar;}
	// default
	return MeasuredData::acmSpecific;
}

void MeasuredDataVectorSettingDialog::setupSolutionComboBox(MeasuredData* data)
{
	ui->solutionComboBox->blockSignals(true);
	for (int i = 0; i < data->pointNames().size(); ++i) {
		ui->scalarComboBox->addItem(data->pointNames().at(i));
		m_scalars.append(data->pointNames().at(i));
	}
	for (int i = 0; i < data->vectorNames().size(); ++i) {
		ui->solutionComboBox->addItem(data->vectorNames().at(i));
		m_solutions.append(data->vectorNames().at(i));
	}
	ui->solutionComboBox->blockSignals(false);
	if (m_solutions.count() <= 1) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
	if (m_scalars.count() == 0) {
		ui->specificRadioButton->isChecked();
		ui->scalarRadioButton->setDisabled(true);
	}
}

void MeasuredDataVectorSettingDialog::setLengthMode(MeasuredData::ArrowLengthMode lm)
{
	if (lm == MeasuredData::almAuto) {
		ui->lengthAutoCheckBox->setChecked(true);
	} else {
		ui->lengthAutoCheckBox->setChecked(false);
	}
}

MeasuredData::ArrowLengthMode MeasuredDataVectorSettingDialog::lengthMode() const
{
	if (ui->lengthAutoCheckBox->isChecked()) {
		return MeasuredData::almAuto;
	} else {
		return MeasuredData::almCustom;
	}
}

void MeasuredDataVectorSettingDialog::setStandardValue(double stdVal)
{
	ui->stdValueSpinBox->setValue(stdVal);
}

double MeasuredDataVectorSettingDialog::standardValue() const
{
	return ui->stdValueSpinBox->value();
}

void MeasuredDataVectorSettingDialog::setLegendLength(int len)
{
	ui->legendLengthSpinBox->setValue(len);
}

int MeasuredDataVectorSettingDialog::legendLength() const
{
	return ui->legendLengthSpinBox->value();
}

void MeasuredDataVectorSettingDialog::setMinimumValue(double minVal)
{
	return ui->minValueSpinBox->setValue(minVal);
}

double MeasuredDataVectorSettingDialog::minimumValue() const
{
	return ui->minValueSpinBox->value();
}
