#ifndef RIVEXPORTER_H
#define RIVEXPORTER_H

#include <QObject>

class CrossSections;

class RivExporter : public QObject
{
	Q_OBJECT

public:
	static bool exportData(const CrossSections& crossSections, QWidget* w);

private:
	RivExporter();
};

#endif // RIVEXPORTER_H
