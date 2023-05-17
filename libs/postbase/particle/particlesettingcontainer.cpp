#include "particlesettingcontainer.h"

ParticleSettingContainer::ParticleSettingContainer() :
	CompositeContainer ({&target, &generateMode, &timeMode, &timeSamplingRate, &timeDivision, &arbitraryTimes}),
	target {"solution"},
	generateMode {"generationMode", GenerateMode::Periodical},
	timeMode {"timeMode", TimeMode::Normal},
	timeSamplingRate {"timeSamplingRate", 2},
	timeDivision {"timeDivision", 2},
	arbitraryTimes {"arbitraryTimes"},
	particleSize {"particleSize", DEFAULT_SIZE}
{}

ParticleSettingContainer::ParticleSettingContainer(const ParticleSettingContainer& c) :
	ParticleSettingContainer {}
{
	copyValue(c);
}

ParticleSettingContainer& ParticleSettingContainer::operator=(const ParticleSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& ParticleSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ParticleSettingContainer&> (c));
}
