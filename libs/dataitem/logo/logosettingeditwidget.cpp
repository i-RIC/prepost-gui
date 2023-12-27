#include "logosetting.h"
#include "logosettingeditwidget.h"
#include "ui_logosettingeditwidget.h"

#include <misc/valuemodifycommandt.h>
#include <misc/qundocommandhelper.h>

LogoSettingEditWidget::LogoSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	m_setting {nullptr},
	ui(new Ui::LogoSettingEditWidget)
{
	ui->setupUi(this);
}

LogoSettingEditWidget::~LogoSettingEditWidget()
{
	delete ui;
}

LogoSetting LogoSettingEditWidget::setting() const
{
	LogoSetting ret;
	ret.imageSetting = ui->imageSettingWidget->setting();
	return ret;
}

void LogoSettingEditWidget::setSetting(LogoSetting* setting)
{
	m_setting = setting;
	setSetting(*setting);

	connect(&m_setting->imageSetting, &ImageSettingContainer::updated, this, &LogoSettingEditWidget::updateImageSetting);
}

void LogoSettingEditWidget::setSetting(const LogoSetting& setting)
{
	ui->imageSettingWidget->setSetting(setting.imageSetting);
}

QUndoCommand* LogoSettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<LogoSetting> (iRIC::generateCommandId("LogoSetting"), apply, setting(), m_setting);
}

void LogoSettingEditWidget::updateImageSetting()
{
	ui->imageSettingWidget->setSetting(m_setting->imageSetting);
}
