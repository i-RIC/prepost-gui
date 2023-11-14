#include "particledatasetting.h"

const int ParticleDataSetting::DEFAULT_SIZE = 3;

ParticleDataSetting::ParticleDataSetting() :
	CompositeContainer({&mapping, &color, &value, &opacity, &particleSize}),
	mapping {"mapping", Mapping::Arbitrary},
	color {"color", Qt::black},
	value {"value", ""},
	opacity {"opacity", 100},
	particleSize {"size", DEFAULT_SIZE}
{}

ParticleDataSetting::ParticleDataSetting(const ParticleDataSetting& setting) :
	ParticleDataSetting {}
{
	copyValue(setting);
}

ParticleDataSetting& ParticleDataSetting::operator=(const ParticleDataSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& ParticleDataSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ParticleDataSetting&> (c));
}

void ParticleDataSetting::copyValue(const XmlAttributeContainer& c)
{
	CompositeContainer::copyValue(c);
	emit updated();
}
