#ifndef COLORMAPSETTINGTOOLBARWIDGET_H
#define COLORMAPSETTINGTOOLBARWIDGET_H

#include "../guicore_global.h"
#include "colormapsettingtoolbarwidgeti.h"

namespace Ui {
class ColorMapSettingToolBarWidget;
}

class GUICOREDLL_EXPORT ColorMapSettingToolBarWidget : public ColorMapSettingToolBarWidgetI
{
	Q_OBJECT

public:
	explicit ColorMapSettingToolBarWidget(QWidget *parent);
	~ColorMapSettingToolBarWidget();

	ColorMapSettingContainerI* modifiedSetting() override;

private slots:
	void applySetting() override;

private:
	Ui::ColorMapSettingToolBarWidget *ui;
};

#endif // COLORMAPSETTINGTOOLBARWIDGET_H
