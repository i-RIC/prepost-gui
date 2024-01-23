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

	if (ui->smallRadioButton->isChecked()) {
		ret.imageSize = LogoSetting::ImageSize::Small;
	} else if (ui->standardRadioButton->isChecked()) {
		ret.imageSize = LogoSetting::ImageSize::Standard;
	} else if (ui->largeRadioButton->isChecked()) {
		ret.imageSize = LogoSetting::ImageSize::Large;
	}

	ret.imageSetting = ui->imageSettingWidget->setting();
	ret.imageSetting.setAutoSizeForced(true);
	ret.imageSetting.setNegativePositionForbidden(true);
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
	if (setting.imageSize == LogoSetting::ImageSize::Small) {
		ui->smallRadioButton->setChecked(true);
	} else if (setting.imageSize == LogoSetting::ImageSize::Standard) {
		ui->standardRadioButton->setChecked(true);
	} else if (setting.imageSize == LogoSetting::ImageSize::Large) {
		ui->largeRadioButton->setChecked(true);
	}

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
