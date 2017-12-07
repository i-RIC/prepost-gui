#ifndef RIVEXPORTER_H
#define RIVEXPORTER_H

#include <QObject>
#include <QString>

class Project;

class RivExporter : public QObject
{
	Q_OBJECT

public:
	RivExporter();

    bool exportData(Project* project, QWidget* w);
    bool exportCsvData(Project* project, QWidget* w);
};

#endif // RIVEXPORTER_H
