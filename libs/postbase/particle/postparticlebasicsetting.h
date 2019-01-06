#ifndef POSTPARTICLEBASICSETTING_H
#define POSTPARTICLEBASICSETTING_H

#include "../postbase_global.h"

#include <misc/compositecontainer.h>
#include <misc/colorcontainer.h>
#include <misc/intcontainer.h>

class POSTBASEDLL_EXPORT PostParticleBasicSetting : public CompositeContainer
{
public:
	PostParticleBasicSetting();
	PostParticleBasicSetting(const PostParticleBasicSetting& s);

	PostParticleBasicSetting& operator=(const PostParticleBasicSetting& s);

	ColorContainer color;
	IntContainer size;
};

#endif // POSTPARTICLEBASICSETTING_H
