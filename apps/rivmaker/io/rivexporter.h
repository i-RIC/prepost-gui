#ifndef RIVEXPORTER_H
#define RIVEXPORTER_H

#include <QObject>

class Project;

class RivExporter : public QObject
{
	Q_OBJECT

public:
	static bool exportData(const Project& project, QWidget* w);

private:
	RivExporter();
};

#endif // RIVEXPORTER_H
