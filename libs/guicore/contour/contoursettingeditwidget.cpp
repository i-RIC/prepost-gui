#include "contoursettingcontainer.h"
#include "contoursettingeditwidget.h"
#include "ui_contoursettingeditwidget.h"

#include <misc/valuemodifycommandt.h>
#include <misc/qundocommandhelper.h>

ContourSettingEditWidget::ContourSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	m_setting {nullptr},
	ui(new Ui::ContourSettingEditWidget)
{
	ui->setupUi(this);
}

ContourSettingEditWidget::~ContourSettingEditWidget()
{
	delete ui;
}

ContourSettingContainer ContourSettingEditWidget::setting() const
{
	ContourSettingContainer c;
	c.drawContourLines = ui->contoursCheckBox->isChecked();
	c.contourLineWidth = ui->widthSpinBox->value();
	if (ui->useColorMapRadioButton->isChecked()) {
		c.mode = ContourSettingContainer::Mode::UseColorMapValues;
	} else if (ui->numberOfValuesRadioButton->isChecked()) {
		c.mode = ContourSettingContainer::Mode::SpecifyNumberOfValues;
	}
	c.numberOfValues = ui->numberOfValuesSpinBox->value();

	return c;
}

void ContourSettingEditWidget::setSetting(const ContourSettingContainer& setting)
{
	ui->contoursCheckBox->setChecked(setting.drawContourLines);
	ui->widthSpinBox->setValue(setting.contourLineWidth);
	if (setting.mode == ContourSettingContainer::Mode::UseColorMapValues) {
		ui->useColorMapRadioButton->setChecked(true);
	} else if (setting.mode == ContourSettingContainer::Mode::SpecifyNumberOfValues) {
		ui->numberOfValuesRadioButton->setChecked(true);
	}
	ui->numberOfValuesSpinBox->setValue(setting.numberOfValues);
}

void ContourSettingEditWidget::setSetting(ContourSettingContainer* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

QUndoCommand* ContourSettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<ContourSettingContainer> (iRIC::generateCommandId("ContourSettingEditWidget::Modify"), apply, setting(), m_setting);
}
