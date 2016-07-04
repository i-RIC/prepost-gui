#include "installertool.h"

#include <misc/iricrootpath.h>

#include <QApplication>
#include <QDir>
#include <QProcess>

InstallerTool::InstallerTool()
{}

InstallerTool::~InstallerTool()
{}

void InstallerTool::openMaintainanceDialog()
{
	QProcess process;
	QStringList args;
	args << "--skip-gui";
	process.start(installerFileName());
	bool finished = false;
	while (! finished) {
		finished = process.waitForFinished(100);
	}
}

QString InstallerTool::installerFileName()
{
	QDir guiDir(iRICRootPath::get());
	// move to installer folder.
	guiDir.cdUp();
	guiDir.cdUp();
	guiDir.cd("installer");
	return guiDir.absoluteFilePath("installer.exe");
}
