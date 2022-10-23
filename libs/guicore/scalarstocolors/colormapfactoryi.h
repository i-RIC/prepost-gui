#ifndef COLORMAPFACTORYI_H
#define COLORMAPFACTORYI_H

#include "../guicore_global.h"

class ColorMapSettingContainerI;
class ColorMapSettingEditWidgetI;

class QWidget;

class GUICOREDLL_EXPORT ColorMapFactoryI
{
public:
	virtual ~ColorMapFactoryI() {}

	virtual ColorMapSettingContainerI* createSettingContainer() = 0;
	virtual ColorMapSettingEditWidgetI* createSettingEditWidget(QWidget* parent) = 0;
};

#endif // COLORMAPFACTORYI_H
