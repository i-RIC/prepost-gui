#include "region2dsettingcontainer.h"
#include "region2dsettingeditwidget.h"
#include "ui_region2dsettingeditwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Region2dSettingEditWidget::Region2dSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	m_setting {nullptr},
	ui(new Ui::Region2dSettingEditWidget)
{
	ui->setupUi(this);

	connect(ui->iMinSlider, &SliderWithValue::valueChanged, this, &Region2dSettingEditWidget::handleIMinChange);
	connect(ui->iMaxSlider, &SliderWithValue::valueChanged, this, &Region2dSettingEditWidget::handleIMaxChange);
	connect(ui->jMinSlider, &SliderWithValue::valueChanged, this, &Region2dSettingEditWidget::handleJMinChange);
	connect(ui->jMaxSlider, &SliderWithValue::valueChanged, this, &Region2dSettingEditWidget::handleJMaxChange);

	connect(ui->fullRadioButton, &QRadioButton::clicked, [=](bool) {emit modeChanged();});
	connect(ui->activeRadioButton, &QRadioButton::clicked, [=](bool) {emit modeChanged();});
	connect(ui->customRadioButton, &QRadioButton::clicked, [=](bool) {emit modeChanged();});
}

Region2dSettingEditWidget::~Region2dSettingEditWidget()
{
	delete ui;
}

void Region2dSettingEditWidget::disableActive()
{
	ui->activeRadioButton->setDisabled(true);
}

void Region2dSettingEditWidget::hideCustom()
{
	ui->customRadioButton->setHidden(true);
	ui->iMinLabel->setHidden(true);
	ui->iMinSlider->setHidden(true);
	ui->iMaxLabel->setHidden(true);
	ui->iMaxSlider->setHidden(true);
	ui->jMinLabel->setHidden(true);
	ui->jMinSlider->setHidden(true);
	ui->jMaxLabel->setHidden(true);
	ui->jMaxSlider->setHidden(true);

	if (! ui->activeRadioButton->isEnabled()) {
		ui->fullRadioButton->setDisabled(true);
	}
}

void Region2dSettingEditWidget::setDimensions(int idim, int jdim)
{
	ui->iMinSlider->setRange(1, idim);
	ui->iMaxSlider->setRange(1, idim);
	ui->jMinSlider->setRange(1, jdim);
	ui->jMaxSlider->setRange(1, jdim);
}

QUndoCommand* Region2dSettingEditWidget::createModifyCommand()
{
	return new ValueModifyCommmand<Region2dSettingContainer>
			(iRIC::generateCommandId("Region2dSettingEditWidget"), true, setting(), m_setting);
}

Region2dSettingContainer Region2dSettingEditWidget::setting()
{
	Region2dSettingContainer ret;

	if (ui->fullRadioButton->isChecked()) {
		ret.mode = Region2dSettingContainer::Mode::Full;
	} else if (ui->activeRadioButton->isChecked()) {
		ret.mode = Region2dSettingContainer::Mode::Active;
	} else if (ui->customRadioButton->isChecked()) {
		ret.mode = Region2dSettingContainer::Mode::Custom;
		ret.iMin = ui->iMinSlider->value() - 1;
		ret.iMax = ui->iMaxSlider->value() - 1;
		ret.jMin = ui->jMinSlider->value() - 1;
		ret.jMax = ui->jMaxSlider->value() - 1;
	}

	return ret;
}

void Region2dSettingEditWidget::setSetting(Region2dSettingContainer* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

void Region2dSettingEditWidget::handleIMinChange(int value)
{
	if (value > ui->iMaxSlider->value()) {
		ui->iMaxSlider->setValue(value);
	}
}

void Region2dSettingEditWidget::handleIMaxChange(int value)
{
	if (value < ui->iMinSlider->value()) {
		ui->iMinSlider->setValue(value);
	}
}

void Region2dSettingEditWidget::handleJMinChange(int value)
{
	if (value > ui->jMaxSlider->value()) {
		ui->jMaxSlider->setValue(value);
	}
}

void Region2dSettingEditWidget::handleJMaxChange(int value)
{
	if (value < ui->jMinSlider->value()) {
		ui->jMinSlider->setValue(value);
	}
}

void Region2dSettingEditWidget::setSetting(const Region2dSettingContainer& setting)
{
	ui->iMinSlider->setValue(ui->iMinSlider->minimum());
	ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
	ui->jMinSlider->setValue(ui->jMinSlider->minimum());
	ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());

	if (setting.mode == Region2dSettingContainer::Mode::Full) {
		ui->fullRadioButton->setChecked(true);
	} else if (setting.mode == Region2dSettingContainer::Mode::Active) {
		ui->activeRadioButton->setChecked(true);
	} else if (setting.mode == Region2dSettingContainer::Mode::Custom) {
		ui->customRadioButton->setChecked(true);
		ui->iMinSlider->setValue(setting.iMin + 1);
		ui->iMaxSlider->setValue(setting.iMax + 1);
		ui->jMinSlider->setValue(setting.jMin + 1);
		ui->jMaxSlider->setValue(setting.jMax + 1);
	}
}
