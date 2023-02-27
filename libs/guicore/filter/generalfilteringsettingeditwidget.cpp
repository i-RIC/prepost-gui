#include "generalfilteringsettingcontainer.h"
#include "generalfilteringsettingeditwidget.h"
#include "ui_generalfilteringsettingeditwidget.h"

#include <misc/valuemodifycommandt.h>

GeneralFilteringSettingEditWidget::GeneralFilteringSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	m_setting {nullptr},
	ui(new Ui::GeneralFilteringSettingEditWidget)
{
	ui->setupUi(this);
}

GeneralFilteringSettingEditWidget::~GeneralFilteringSettingEditWidget()
{
	delete ui;
}

void GeneralFilteringSettingEditWidget::setSetting(GeneralFilteringSettingContainer* setting)
{
	m_setting = setting;
	if (setting->samplingMode == GeneralFilteringSettingContainer::SamplingMode::All) {
		ui->samplingAllRadioButton->setChecked(true);
	} else if (setting->samplingMode == GeneralFilteringSettingContainer::SamplingMode::Rate) {
		ui->samplingRateRadioButton->setChecked(true);
	} else if (setting->samplingMode == GeneralFilteringSettingContainer::SamplingMode::Number) {
		ui->samplingNumberRadioButton->setChecked(true);
	}
	ui->samplingRateSpinBox->setValue(setting->samplingRate);
	ui->samplingNumberSpinBox->setValue(setting->samplingNumber);
}

GeneralFilteringSettingContainer GeneralFilteringSettingEditWidget::setting() const
{
	GeneralFilteringSettingContainer ret;
	if (ui->samplingAllRadioButton->isChecked()) {
		ret.samplingMode = GeneralFilteringSettingContainer::SamplingMode::All;
	} else if (ui->samplingRateRadioButton->isChecked()) {
		ret.samplingMode = GeneralFilteringSettingContainer::SamplingMode::Rate;
	} else if (ui->samplingNumberRadioButton->isChecked()) {
		ret.samplingMode = GeneralFilteringSettingContainer::SamplingMode::Number;
	}
	ret.samplingRate = ui->samplingRateSpinBox->value();
	ret.samplingNumber = ui->samplingNumberSpinBox->value();

	return ret;
}

QUndoCommand* GeneralFilteringSettingEditWidget::createModifyCommand(QUndoCommand* parent) const
{
	return new ValueModifyCommmand<GeneralFilteringSettingContainer>("Modify  filtering setting", setting(), m_setting, parent);
}
