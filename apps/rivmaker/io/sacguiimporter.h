#ifndef SACGUIIMPORTER_H
#define SACGUIIMPORTER_H

#include <QObject>

class Project;

class SACGUIImporter : public QObject
{
	Q_OBJECT

public:
	static bool importData(Project* project, QWidget* w);

private:
	SACGUIImporter();
};

#endif // SACGUIIMPORTER_H
