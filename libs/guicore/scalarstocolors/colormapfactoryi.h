#ifndef COLORMAPFACTORYI_H
#define COLORMAPFACTORYI_H

#include "../guicore_global.h"

class ColorMapLegendSettingContainerI;
class ColorMapSettingContainerI;
class ColorMapSettingEditWidgetI;
class ColorMapSettingToolBarWidgetI;

class QWidget;

class GUICOREDLL_EXPORT ColorMapFactoryI
{
public:
	virtual ~ColorMapFactoryI() {}

	virtual ColorMapSettingContainerI* createSettingContainer() = 0;
	virtual ColorMapLegendSettingContainerI* createLegendSettingContainer() = 0;
	virtual ColorMapSettingEditWidgetI* createSettingEditWidget(QWidget* parent) = 0;
	virtual ColorMapSettingToolBarWidgetI* createToolbarWidget(QWidget* parent) = 0;
};

#endif // COLORMAPFACTORYI_H
