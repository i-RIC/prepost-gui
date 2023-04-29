#ifndef COLORMAPSETTINGTOOLBARWIDGETCONTROLLER_H
#define COLORMAPSETTINGTOOLBARWIDGETCONTROLLER_H

#include "../guicore_global.h"

#include <QObject>

class ColorMapSettingToolBarWidget;

class GUICOREDLL_EXPORT ColorMapSettingToolBarWidgetController : public QObject
{
public:
	ColorMapSettingToolBarWidgetController(ColorMapSettingToolBarWidget* widget);
	~ColorMapSettingToolBarWidgetController();

	ColorMapSettingToolBarWidget* widget() const;

private slots:
	virtual void handleWidgetUpdate() = 0;

protected:
	ColorMapSettingToolBarWidget* m_widget;
};

#endif // COLORMAPSETTINGTOOLBARWIDGETCONTROLLER_H
