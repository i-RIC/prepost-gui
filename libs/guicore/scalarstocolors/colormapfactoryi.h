#ifndef COLORMAPFACTORYI_H
#define COLORMAPFACTORYI_H

#include "../guicore_global.h"

class ColorMapLegendSettingContainerI;
class ColorMapSettingContainerI;
class ColorMapSettingEditWidgetI;
class ColorMapSettingToolBarWidget;

class QWidget;

class GUICOREDLL_EXPORT ColorMapFactoryI
{
public:
	virtual ~ColorMapFactoryI() {}

	virtual ColorMapSettingContainerI* createSettingContainer() = 0;
	virtual ColorMapLegendSettingContainerI* createLegendSettingContainer() = 0;
	virtual ColorMapSettingEditWidgetI* createSettingEditWidget(QWidget* parent) = 0;
	virtual ColorMapSettingToolBarWidget* createToolbarWidget(QWidget* parent) = 0;
};

#endif // COLORMAPFACTORYI_H
