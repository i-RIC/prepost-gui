#ifndef AUTOPARTICLEWINDOWI_H
#define AUTOPARTICLEWINDOWI_H

/// Window that output particles automatically generated in post window
class AutoParticleWindowI
{
public:
	virtual ~AutoParticleWindowI() {}

	virtual bool isAutoParticleOutput() const = 0;
};

#endif // AUTOPARTICLEWINDOWI_H
