#ifndef COLORMAPSETTINGTOOLBARWIDGETI_H
#define COLORMAPSETTINGTOOLBARWIDGETI_H

#include "../guicore_global.h"

class ColorMapSettingContainerI;

class GUICOREDLL_EXPORT ColorMapSettingToolBarWidgetI : public QWidget
{
	Q_OBJECT

public:
	ColorMapSettingToolBarWidgetI(QWidget* parent);
	virtual ~ColorMapSettingToolBarWidgetI();

	void setSetting(ColorMapSettingContainerI* setting);

	virtual ColorMapSettingContainerI* modifiedSetting() = 0;

signals:
	void updated();

private slots:
	virtual void applySetting() = 0;

protected:
	ColorMapSettingContainerI* m_setting;
};

#endif // COLORMAPSETTINGTOOLBARWIDGETI_H
