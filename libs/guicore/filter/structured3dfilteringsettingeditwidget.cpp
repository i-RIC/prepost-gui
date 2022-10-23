#include "structured3dfilteringsettingcontainer.h"
#include "structured3dfilteringsettingeditwidget.h"
#include "ui_structured3dfilteringsettingeditwidget.h"

#include <misc/valuemodifycommandt.h>
#include <misc/qundocommandhelper.h>

Structured3dFilteringSettingEditWidget::Structured3dFilteringSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	m_setting {nullptr},
	ui(new Ui::Structured3dFilteringSettingEditWidget)
{
	ui->setupUi(this);
}

Structured3dFilteringSettingEditWidget::~Structured3dFilteringSettingEditWidget()
{
	delete ui;
}

void Structured3dFilteringSettingEditWidget::setSetting(Structured3dFilteringSettingContainer* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

QUndoCommand* Structured3dFilteringSettingEditWidget::createModifyCommand()
{
	return new ValueModifyCommmand<Structured3dFilteringSettingContainer>(iRIC::generateCommandId("Structured3dFilteringSettingEditWidget::ModifyCommand"), true,
																 setting(), m_setting);
}

Structured3dFilteringSettingContainer Structured3dFilteringSettingEditWidget::setting() const
{
	Structured3dFilteringSettingContainer ret;
	if (ui->samplingAllRadioButton->isChecked()) {
		ret.samplingMode = Structured3dFilteringSettingContainer::SamplingMode::All;
	} else {
		ret.samplingMode = Structured3dFilteringSettingContainer::SamplingMode::Rate;
		ret.iSampleRate = ui->iSamplingRateSpinBox->value();
		ret.jSampleRate = ui->jSamplingRateSpinBox->value();
		ret.kSampleRate = ui->kSamplingRateSpinBox->value();
	}

	return ret;
}

void Structured3dFilteringSettingEditWidget::setSetting(const Structured3dFilteringSettingContainer& setting)
{
	if (setting.samplingMode == Structured3dFilteringSettingContainer::SamplingMode::All) {
		ui->samplingAllRadioButton->setChecked(true);
		ui->iSamplingRateSpinBox->setValue(1);
		ui->jSamplingRateSpinBox->setValue(1);
		ui->kSamplingRateSpinBox->setValue(1);
	} else {
		ui->samplingSkipRadioButton->setChecked(true);
		ui->iSamplingRateSpinBox->setValue(setting.iSampleRate);
		ui->jSamplingRateSpinBox->setValue(setting.jSampleRate);
		ui->kSamplingRateSpinBox->setValue(setting.kSampleRate);
	}
}
