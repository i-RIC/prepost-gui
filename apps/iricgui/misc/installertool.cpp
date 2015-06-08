#include "installertool.h"

#include <QApplication>
#include <QDir>
#include <QProcess>

InstallerTool::InstallerTool()
{

}

InstallerTool::~InstallerTool()
{

}


void InstallerTool::openMaintainanceDialog()
{
	QProcess process;
	process.start(installerFileName());
	bool finished = false;
	while (! finished) {
		finished = process.waitForFinished(100);
	}
}

QString InstallerTool::installerFileName()
{
	QDir guiDir(QCoreApplication::applicationDirPath());
	// move to installer folder.
	guiDir.cdUp();
	guiDir.cdUp();
	guiDir.cd("installer");
	return guiDir.absoluteFilePath("installer.exe");
}
