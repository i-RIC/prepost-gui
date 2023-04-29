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

	connect(ui->fullRadioButton, &QRadioButton::clicked, [=]() {emit modeChanged();});
	connect(ui->activeRadioButton, &QRadioButton::clicked, [=]() {emit modeChanged();});
	connect(ui->customRadioButton, &QRadioButton::clicked, [=]() {emit modeChanged();});
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
	ui->rangeWidget->setHidden(true);

	if (! ui->activeRadioButton->isEnabled()) {
		ui->fullRadioButton->setDisabled(true);
	}
}

void Region2dSettingEditWidget::setDimensions(int idim, int jdim)
{
	ui->rangeWidget->setDimensions(idim, jdim);
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
		ret.range = ui->rangeWidget->setting();
	}

	return ret;
}

void Region2dSettingEditWidget::setSetting(Region2dSettingContainer* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

void Region2dSettingEditWidget::setSetting(const Region2dSettingContainer& setting)
{
	ui->rangeWidget->setupFullRange();

	if (setting.mode == Region2dSettingContainer::Mode::Full) {
		ui->fullRadioButton->setChecked(true);
	} else if (setting.mode == Region2dSettingContainer::Mode::Active) {
		ui->activeRadioButton->setChecked(true);
	} else if (setting.mode == Region2dSettingContainer::Mode::Custom) {
		ui->customRadioButton->setChecked(true);
		ui->rangeWidget->setSetting(setting.range);
	}
}
