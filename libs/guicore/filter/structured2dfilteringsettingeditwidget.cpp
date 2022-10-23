#include "structured2dfilteringsettingcontainer.h"
#include "structured2dfilteringsettingeditwidget.h"
#include "ui_structured2dfilteringsettingeditwidget.h"

#include <misc/valuemodifycommandt.h>
#include <misc/qundocommandhelper.h>

Structured2dFilteringSettingEditWidget::Structured2dFilteringSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Structured2dFilteringSettingEditWidget)
{
	ui->setupUi(this);
}

Structured2dFilteringSettingEditWidget::~Structured2dFilteringSettingEditWidget()
{
	delete ui;
}

void Structured2dFilteringSettingEditWidget::setSetting(Structured2dFilteringSettingContainer* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

void Structured2dFilteringSettingEditWidget::setModeToAll()
{
	ui->samplingAllRadioButton->setChecked(true);
}

QUndoCommand* Structured2dFilteringSettingEditWidget::createModifyCommand()
{
	return new ValueModifyCommmand<Structured2dFilteringSettingContainer>(iRIC::generateCommandId("Structured2dFilteringSettingEditWidget::ModifyCommand"), true,
																 setting(), m_setting);
}

Structured2dFilteringSettingContainer Structured2dFilteringSettingEditWidget::setting() const
{
	Structured2dFilteringSettingContainer ret;
	if (ui->samplingAllRadioButton->isChecked()) {
		ret.samplingMode = Structured2dFilteringSettingContainer::SamplingMode::All;
	} else {
		ret.samplingMode = Structured2dFilteringSettingContainer::SamplingMode::Rate;
		ret.iSampleRate = ui->iSamplingRateSpinBox->value();
		ret.jSampleRate = ui->jSamplingRateSpinBox->value();
	}

	return ret;
}

void Structured2dFilteringSettingEditWidget::setSetting(const Structured2dFilteringSettingContainer& setting)
{
	if (setting.samplingMode == Structured2dFilteringSettingContainer::SamplingMode::All) {
		ui->samplingAllRadioButton->setChecked(true);
		ui->iSamplingRateSpinBox->setValue(1);
		ui->jSamplingRateSpinBox->setValue(1);
	} else {
		ui->samplingSkipRadioButton->setChecked(true);
		ui->iSamplingRateSpinBox->setValue(setting.iSampleRate);
		ui->jSamplingRateSpinBox->setValue(setting.jSampleRate);
	}
}
