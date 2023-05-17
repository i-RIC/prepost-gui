#ifndef PARTICLESETTINGCONTAINER_H
#define PARTICLESETTINGCONTAINER_H

#include "../postbase_global.h"

#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intarraycontainer.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>

class POSTBASEDLL_EXPORT ParticleSettingContainer : public CompositeContainer
{
public:
	static const int DEFAULT_SIZE = 3;

	enum class GenerateMode {
		Periodical, Arbitrary
	};

	enum class TimeMode {
		Normal,
		Subdivide,
		Skip
	};

	ParticleSettingContainer();
	ParticleSettingContainer(const ParticleSettingContainer& c);

	ParticleSettingContainer& operator=(const ParticleSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	StringContainer target;
	EnumContainerT<GenerateMode> generateMode;
	EnumContainerT<TimeMode> timeMode;

	IntContainer timeSamplingRate;
	IntContainer timeDivision;
	IntArrayContainer arbitraryTimes;
	IntContainer particleSize;
};

#endif // PARTICLESETTINGCONTAINER_H
