#ifndef COLORMAPENUMERATEFACTORY_H
#define COLORMAPENUMERATEFACTORY_H

#include "../guicore_global.h"
#include "colormapfactoryi.h"

class GUICOREDLL_EXPORT ColorMapEnumerateFactory : public ColorMapFactoryI
{
public:
	ColorMapSettingContainerI* createSettingContainer() override;
	ColorMapLegendSettingContainerI* createLegendSettingContainer() override;
	ColorMapSettingEditWidgetI* createSettingEditWidget(QWidget* parent) override;
	ColorMapSettingToolBarWidget* createToolbarWidget(QWidget* parent) override;
};

#endif // COLORMAPENUMERATEFACTORY_H
