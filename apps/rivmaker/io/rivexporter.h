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

	bool exportData(const Project& project, QWidget* w);
	bool exportCsvData(const Project& project, QWidget* w);

private:
	QString m_rivFileName;
	QString m_csvFileName;
};

#endif // RIVEXPORTER_H
