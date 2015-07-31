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

MeasuredDataVectorGroupDataItem::Setting MeasuredDataVectorSettingDialog::setting() const
{
	MeasuredDataVectorGroupDataItem::Setting ret = m_setting;

	// solution
	int index = ui->solutionComboBox->currentIndex();
	if (index < 0 || index > m_solutions.count() - 1) {ret.currentSolution = "";}
	else {ret.currentSolution = m_solutions.at(index);}

	// scalar
	index = ui->scalarComboBox->currentIndex();
	if (index < 0 || index > m_scalars.count() - 1) {ret.scalarValueName = "";}
	else {ret.scalarValueName = m_scalars.at(index);}

	// color
	ret.color = ui->colorEditWidget->color();

	// arrowcolormode
	if (ui->specificRadioButton->isChecked()) {ret.colorMode = MeasuredData::acmSpecific;}
	else if (ui->scalarRadioButton->isChecked()) {ret.colorMode = MeasuredData::acmScalar;}
	else {ret.colorMode = MeasuredData::acmSpecific;}

	// lengthmode
	if (ui->lengthAutoCheckBox->isChecked()) {
		ret.lengthMode = MeasuredData::almAuto;
	} else {
		ret.lengthMode = MeasuredData::almCustom;
	}

	// standardValue
	ret.standardValue = ui->stdValueSpinBox->value();

	// legendLength
	ret.legendLength = ui->legendLengthSpinBox->value();

	// minimumValue
	ret.minimumValue = ui->minValueSpinBox->value();

	return ret;
}

void MeasuredDataVectorSettingDialog::setSetting(const MeasuredDataVectorGroupDataItem::Setting& setting)
{
	m_setting = setting;
	// solution
	int index = m_solutions.indexOf(setting.currentSolution);
	if (index == -1) {index = 0;}
	ui->solutionComboBox->setCurrentIndex(index);

	// scalar
	index = m_scalars.indexOf(setting.scalarValueName);
	if (index == -1) { index = 0; }
	ui->scalarComboBox->setCurrentIndex(index);

	// color
	ui->colorEditWidget->setColor(setting.color);

	// arrowcolormode
	switch (MeasuredData::ArrowColorMode(setting.colorMode)){
	case MeasuredData::acmSpecific:
		ui->specificRadioButton->setChecked(true);
		break;
	case MeasuredData::acmScalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}

	// lengthmode
	if (setting.lengthMode == MeasuredData::almAuto) {
		ui->lengthAutoCheckBox->setChecked(true);
	} else {
		ui->lengthAutoCheckBox->setChecked(false);
	}

	// standardValue
	ui->stdValueSpinBox->setValue(setting.standardValue);

	// legendLength
	ui->legendLengthSpinBox->setValue(setting.legendLength);

	// minimumValue
	ui->minValueSpinBox->setValue(setting.minimumValue);
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
