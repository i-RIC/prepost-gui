#ifndef COLORMAPENUMERATEFACTORY_H
#define COLORMAPENUMERATEFACTORY_H

#include "../guicore_global.h"
#include "colormapfactoryi.h"

class ColorMapEnumerateFactory : public ColorMapFactoryI
{
public:
	ColorMapEnumerateFactory();

	ColorMapSettingContainerI* createSettingContainer() override;
	ColorMapSettingEditWidgetI* createSettingEditWidget(QWidget* parent) override;
};

#endif // COLORMAPENUMERATEFACTORY_H
