#ifndef POSTPARTICLEBASICSETTING_H
#define POSTPARTICLEBASICSETTING_H

#include "../postbase_global.h"

#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/colorcontainer.h>

class POSTBASEDLL_EXPORT PostParticleBasicSetting : public CompositeContainer
{
public:
	PostParticleBasicSetting();
	PostParticleBasicSetting(const PostParticleBasicSetting& s);

	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	ColorContainer color;
	IntContainer size;
};

#endif // POSTPARTICLEBASICSETTING_H
