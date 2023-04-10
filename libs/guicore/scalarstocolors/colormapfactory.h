#ifndef COLORMAPFACTORY_H
#define COLORMAPFACTORY_H

#include "../guicore_global.h"
#include "colormapfactoryi.h"

class GUICOREDLL_EXPORT ColorMapFactory : public ColorMapFactoryI
{
public:
	ColorMapSettingContainerI* createSettingContainer() override;
	ColorMapLegendSettingContainerI* createLegendSettingContainer() override;
	ColorMapSettingEditWidgetI* createSettingEditWidget(QWidget* parent) override;
};

#endif // COLORMAPFACTORY_H
