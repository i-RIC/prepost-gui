#include "gridattributenodesetting.h"
#include "gridattributenodesettingeditwidget.h"
#include "ui_gridattributenodesettingeditwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GridAttributeNodeSettingEditWidget::GridAttributeNodeSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	m_setting {nullptr},
	ui(new Ui::GridAttributeNodeSettingEditWidget)
{
	ui->setupUi(this);
}

GridAttributeNodeSettingEditWidget::~GridAttributeNodeSettingEditWidget()
{
	delete ui;
}

GridAttributeNodeSetting GridAttributeNodeSettingEditWidget::setting() const
{
	GridAttributeNodeSetting ret;

	if (ui->paintRadioButton->isChecked()) {
		ret.renderMode = GridAttributeNodeSetting::RenderMode::Paint;
	} else if (ui->pointsRadioButton->isChecked()) {
		ret.renderMode = GridAttributeNodeSetting::RenderMode::Point;
	}
	ret.lineWidth = ui->lineWidthSpinBox->value();
	ret.pointSize = ui->pointSizeSpinBox->value();
	ret.opacity = ui->opacityWidget->opacity();

	return ret;
}

void GridAttributeNodeSettingEditWidget::setSetting(const GridAttributeNodeSetting& setting)
{
	if (setting.renderMode == GridAttributeNodeSetting::RenderMode::Paint) {
		ui->paintRadioButton->setChecked(true);
	} else if (setting.renderMode == GridAttributeNodeSetting::RenderMode::Point) {
		ui->pointsRadioButton->setChecked(true);
	}
	ui->lineWidthSpinBox->setValue(setting.lineWidth);
	ui->pointSizeSpinBox->setValue(setting.pointSize);
	ui->opacityWidget->setOpacity(setting.opacity);
}

void GridAttributeNodeSettingEditWidget::setSetting(GridAttributeNodeSetting* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

void GridAttributeNodeSettingEditWidget::hideLineWidth()
{
	ui->lineWidthLabel->hide();
	ui->lineWidthSpinBox->hide();
}

QUndoCommand* GridAttributeNodeSettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<GridAttributeNodeSetting>(iRIC::generateCommandId("GridAttributeNodeSettingEditWidget"), apply, setting(), m_setting);
}
