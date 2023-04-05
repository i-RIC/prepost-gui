#ifndef DELEGATEDCOLORMAPSETTINGEDITWIDGET_H
#define DELEGATEDCOLORMAPSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class DelegatedColorMapSettingEditWidget;
}

class ColorMapSettingEditWidgetI;
class DelegatedColorMapSettingContainer;

class GUICOREDLL_EXPORT DelegatedColorMapSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DelegatedColorMapSettingEditWidget(QWidget *parent = nullptr);
	~DelegatedColorMapSettingEditWidget();

	void setEditWidget(ColorMapSettingEditWidgetI* widget);
	void setSetting(DelegatedColorMapSettingContainer* setting);
	QUndoCommand* createModifyCommand(bool apply) override;

private slots:
	void setUseOriginalSetting(bool use);

private:
	ColorMapSettingEditWidgetI* m_widget;
	DelegatedColorMapSettingContainer* m_setting;
	Ui::DelegatedColorMapSettingEditWidget *ui;
};

#endif // DELEGATEDCOLORMAPSETTINGEDITWIDGET_H
