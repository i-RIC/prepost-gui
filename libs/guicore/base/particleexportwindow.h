#ifndef PARTICLEEXPORTWINDOW_H
#define PARTICLEEXPORTWINDOW_H

#include <QList>
#include <QString>

class QString;

class ParticleExportWindow {
public:
	ParticleExportWindow(){}
	virtual bool exportParticles(const QString& filename, int index, double time, const QString& zonename) = 0;
	virtual QList<QString> particleDrawingZones() = 0;
};

#endif // PARTICLEEXPORTWINDOW_H
