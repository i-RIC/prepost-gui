#ifndef LOGOSETTINGEDITWIDGET_H
#define LOGOSETTINGEDITWIDGET_H

#include <misc/modifycommandwidget.h>

namespace Ui {
class LogoSettingEditWidget;
}

class LogoSetting;

class LogoSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit LogoSettingEditWidget(QWidget *parent = nullptr);
	~LogoSettingEditWidget();

	LogoSetting setting() const;
	void setSetting(LogoSetting* setting);
	void setSetting(const LogoSetting& setting);

	QUndoCommand* createModifyCommand(bool apply) override;

private slots:
	void updateImageSetting();

private:
	LogoSetting* m_setting;
	Ui::LogoSettingEditWidget *ui;
};

#endif // LOGOSETTINGEDITWIDGET_H
