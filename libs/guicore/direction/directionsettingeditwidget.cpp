#include "directionsettingcontainer.h"
#include "directionsettingeditwidget.h"
#include "ui_directionsettingeditwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

DirectionSettingEditWidget::DirectionSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	ui(new Ui::DirectionSettingEditWidget)
{
	ui->setupUi(this);
}

DirectionSettingEditWidget::~DirectionSettingEditWidget()
{
	delete ui;
}

DirectionSettingContainer DirectionSettingEditWidget::setting() const
{
	DirectionSettingContainer ret;

	if (ui->mode1to8RadioButton->isChecked()) {
		ret.mode = DirectionSettingContainer::Mode::Mode1to8;
	} else if (ui->mode1to128RadioButton->isChecked()) {
		ret.mode = DirectionSettingContainer::Mode::Mode1to128;
	}
	ret.color = ui->colorWidget->color();
	ret.opacity = ui->opacityWidget->opacity();
	ret.arrowSize = ui->sizeSpinBox->value();
	ret.lineWidth = ui->widthSpinBox->value();

	return ret;
}

void DirectionSettingEditWidget::setSetting(const DirectionSettingContainer& setting)
{
	if (setting.mode == DirectionSettingContainer::Mode::Mode1to8) {
		ui->mode1to8RadioButton->setChecked(true);
	} else if (setting.mode == DirectionSettingContainer::Mode::Mode1to128) {
		ui->mode1to128RadioButton->setChecked(true);
	}
	ui->colorWidget->setColor(setting.color);
	ui->opacityWidget->setOpacity(setting.opacity);
	ui->sizeSpinBox->setValue(setting.arrowSize);
	ui->widthSpinBox->setValue(setting.lineWidth);
}

void DirectionSettingEditWidget::setSetting(DirectionSettingContainer* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

QUndoCommand* DirectionSettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<DirectionSettingContainer> (iRIC::generateCommandId("DirectionSettingEditWidget"), apply, setting(), m_setting);
}
