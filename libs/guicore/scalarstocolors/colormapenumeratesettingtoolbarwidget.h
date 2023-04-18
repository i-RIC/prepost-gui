#ifndef COLORMAPENUMERATESETTINGTOOLBARWIDGET_H
#define COLORMAPENUMERATESETTINGTOOLBARWIDGET_H

#include "../guicore_global.h"
#include "colormapsettingtoolbarwidgeti.h"

namespace Ui {
class ColorMapEnumerateSettingToolBarWidget;
}

class GUICOREDLL_EXPORT ColorMapEnumerateSettingToolBarWidget : public ColorMapSettingToolBarWidgetI
{
	Q_OBJECT

public:
	explicit ColorMapEnumerateSettingToolBarWidget(QWidget *parent);
	~ColorMapEnumerateSettingToolBarWidget();

	ColorMapSettingContainerI* modifiedSetting() override;

private slots:
	void applySetting() override;

private:
	Ui::ColorMapEnumerateSettingToolBarWidget *ui;
};

#endif // COLORMAPENUMERATESETTINGTOOLBARWIDGET_H
