#include "regionrange2dsettingcontainer.h"
#include "regionrange2dsettingeditwidget.h"
#include "ui_regionrange2dsettingeditwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

RegionRange2dSettingEditWidget::RegionRange2dSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RegionRange2dSettingEditWidget)
{
	ui->setupUi(this);

	connect(ui->iMinSlider, &SliderWithValue::valueChanged, this, &RegionRange2dSettingEditWidget::handleIMinChange);
	connect(ui->iMaxSlider, &SliderWithValue::valueChanged, this, &RegionRange2dSettingEditWidget::handleIMaxChange);
	connect(ui->jMinSlider, &SliderWithValue::valueChanged, this, &RegionRange2dSettingEditWidget::handleJMinChange);
	connect(ui->jMaxSlider, &SliderWithValue::valueChanged, this, &RegionRange2dSettingEditWidget::handleJMaxChange);
}

RegionRange2dSettingEditWidget::~RegionRange2dSettingEditWidget()
{
	delete ui;
}

void RegionRange2dSettingEditWidget::setDimensions(int idim, int jdim)
{
	ui->iMinSlider->setRange(1, idim);
	ui->iMaxSlider->setRange(1, idim);
	ui->jMinSlider->setRange(1, jdim);
	ui->jMaxSlider->setRange(1, jdim);
}

void RegionRange2dSettingEditWidget::setupFullRange()
{
	ui->iMinSlider->setValue(ui->iMinSlider->minimum());
	ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
	ui->jMinSlider->setValue(ui->jMinSlider->minimum());
	ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());
}

QUndoCommand* RegionRange2dSettingEditWidget::createModifyCommand(bool allowMerge)
{
	return new ValueModifyCommmand<RegionRange2dSettingContainer>
			(iRIC::generateCommandId("RegionRange2dSettingEditWidget"), allowMerge, setting(), m_setting);
}

RegionRange2dSettingContainer RegionRange2dSettingEditWidget::setting()
{
	RegionRange2dSettingContainer ret;

	ret.iMin = ui->iMinSlider->value() - 1;
	ret.iMax = ui->iMaxSlider->value() - 1;
	ret.jMin = ui->jMinSlider->value() - 1;
	ret.jMax = ui->jMaxSlider->value() - 1;

	return ret;
}

void RegionRange2dSettingEditWidget::setSetting(RegionRange2dSettingContainer* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

void RegionRange2dSettingEditWidget::setSetting(const RegionRange2dSettingContainer& setting)
{
	ui->iMinSlider->setValue(setting.iMin + 1);
	ui->iMaxSlider->setValue(setting.iMax + 1);
	ui->jMinSlider->setValue(setting.jMin + 1);
	ui->jMaxSlider->setValue(setting.jMax + 1);
}

void RegionRange2dSettingEditWidget::handleIMinChange(int value)
{
	if (value > ui->iMaxSlider->value()) {
		ui->iMaxSlider->setValue(value);
	}
}

void RegionRange2dSettingEditWidget::handleIMaxChange(int value)
{
	if (value < ui->iMinSlider->value()) {
		ui->iMinSlider->setValue(value);
	}
}

void RegionRange2dSettingEditWidget::handleJMinChange(int value)
{
	if (value > ui->jMaxSlider->value()) {
		ui->jMaxSlider->setValue(value);
	}
}

void RegionRange2dSettingEditWidget::handleJMaxChange(int value)
{
	if (value < ui->jMinSlider->value()) {
		ui->jMinSlider->setValue(value);
	}
}
