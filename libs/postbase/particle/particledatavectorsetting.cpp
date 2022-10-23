#include "particledatavectorsetting.h"

ParticleDataVectorSetting::ParticleDataVectorSetting() :
	CompositeContainer({&filteringSetting, &arrowsSetting}),
	filteringSetting {},
	arrowsSetting {}
{}

ParticleDataVectorSetting::ParticleDataVectorSetting(const ParticleDataVectorSetting& setting) :
	ParticleDataVectorSetting {}
{
	copyValue(setting);
}

ParticleDataVectorSetting::~ParticleDataVectorSetting()
{}

ParticleDataVectorSetting& ParticleDataVectorSetting::operator=(const ParticleDataVectorSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& ParticleDataVectorSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ParticleDataVectorSetting&> (c));
}
