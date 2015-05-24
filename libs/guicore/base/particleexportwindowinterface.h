#ifndef PARTICLEEXPORTWINDOWINTERFACE_H
#define PARTICLEEXPORTWINDOWINTERFACE_H

#include <QList>
#include <QString>

class QString;

class ParticleExportWindowInterface
{
public:
	virtual bool exportParticles(const QString& filename, int index, double time, const QString& zonename) = 0;
	virtual QList<QString> particleDrawingZones() = 0;
};

#endif // PARTICLEEXPORTWINDOWINTERFACE_H
