#ifndef PARTICLEDATASETTING_H
#define PARTICLEDATASETTING_H

#include "../postbase_global.h"

#include <misc/compositecontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class POSTBASEDLL_EXPORT ParticleDataSetting : public CompositeContainer
{
public:
	enum class Mapping {
		Value,
		Arbitrary
	};

	ParticleDataSetting();
	ParticleDataSetting(const ParticleDataSetting& setting);

	ParticleDataSetting& operator=(const ParticleDataSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<Mapping> mapping;
	ColorContainer color;
	StringContainer value;
	OpacityContainer opacity;
	IntContainer particleSize;
};

#endif // PARTICLEDATASETTING_H
