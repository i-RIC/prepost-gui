#ifndef PARTICLEEXPORTWINDOWI_H
#define PARTICLEEXPORTWINDOWI_H

#include <QList>

class QString;

class ParticleExportWindowI
{
public:
	virtual ~ParticleExportWindowI() {}

	virtual bool exportParticles(const QString& filename, int index, double time, const QString& zonename) = 0;
	virtual QList<QString> particleDrawingZones() = 0;
};

#endif // PARTICLEEXPORTWINDOWI_H
