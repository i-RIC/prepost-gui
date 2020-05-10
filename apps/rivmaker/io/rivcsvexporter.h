#ifndef RIVCSVEXPORTER_H
#define RIVCSVEXPORTER_H

#include <QObject>
#include <QString>

class Project;

class RivCsvExporter : public QObject
{
	Q_OBJECT

public:
	RivCsvExporter();

	bool exportData(Project* project, QWidget* w);
};

#endif // RIVCSVEXPORTER_H
