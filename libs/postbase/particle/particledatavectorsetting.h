#ifndef PARTICLEDATAVECTORSETTING_H
#define PARTICLEDATAVECTORSETTING_H

#include "../postbase_global.h"

#include <guicore/arrows/arrowssettingcontainer.h>
#include <guicore/filter/generalfilteringsettingcontainer.h>
#include <misc/compositecontainer.h>

class POSTBASEDLL_EXPORT ParticleDataVectorSetting : public CompositeContainer
{
public:
	ParticleDataVectorSetting();
	ParticleDataVectorSetting(const ParticleDataVectorSetting& setting);
	~ParticleDataVectorSetting();

	ParticleDataVectorSetting& operator=(const ParticleDataVectorSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	GeneralFilteringSettingContainer filteringSetting;
	ArrowsSettingContainer arrowsSetting;
};

#endif // PARTICLEDATAVECTORSETTING_H
