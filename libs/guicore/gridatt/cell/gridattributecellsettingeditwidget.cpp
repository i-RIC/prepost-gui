#include "gridattributecellsetting.h"
#include "gridattributecellsettingeditwidget.h"
#include "ui_gridattributecellsettingeditwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GridAttributeCellSettingEditWidget::GridAttributeCellSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	m_setting {nullptr},
	ui(new Ui::GridAttributeCellSettingEditWidget)
{
	ui->setupUi(this);
}

GridAttributeCellSettingEditWidget::~GridAttributeCellSettingEditWidget()
{
	delete ui;
}

GridAttributeCellSetting GridAttributeCellSettingEditWidget::setting() const
{
	GridAttributeCellSetting ret;

	ret.lineWidth = ui->lineWidthSpinBox->value();
	ret.opacity = ui->opacityWidget->opacity();

	return ret;
}

void GridAttributeCellSettingEditWidget::setSetting(const GridAttributeCellSetting& setting)
{
	ui->lineWidthSpinBox->setValue(setting.lineWidth);
	ui->opacityWidget->setOpacity(setting.opacity);
}

void GridAttributeCellSettingEditWidget::setSetting(GridAttributeCellSetting* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

void GridAttributeCellSettingEditWidget::hideLineWidth()
{
	ui->lineWidthLabel->hide();
	ui->lineWidthSpinBox->hide();
}

QUndoCommand* GridAttributeCellSettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<GridAttributeCellSetting>(iRIC::generateCommandId("GridAttributeCellSettingEditWidget"), apply, setting(), m_setting);
}
