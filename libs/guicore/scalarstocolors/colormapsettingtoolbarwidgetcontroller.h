#ifndef COLORMAPSETTINGTOOLBARWIDGETCONTROLLER_H
#define COLORMAPSETTINGTOOLBARWIDGETCONTROLLER_H

#include "../guicore_global.h"

#include <QObject>

class ColorMapSettingToolBarWidgetI;

class GUICOREDLL_EXPORT ColorMapSettingToolBarWidgetController : public QObject
{
public:
	ColorMapSettingToolBarWidgetController(ColorMapSettingToolBarWidgetI* widget);
	~ColorMapSettingToolBarWidgetController();

	ColorMapSettingToolBarWidgetI* widget() const;

private slots:
	virtual void handleWidgetUpdate() = 0;

protected:
	ColorMapSettingToolBarWidgetI* m_widget;
};

#endif // COLORMAPSETTINGTOOLBARWIDGETCONTROLLER_H
