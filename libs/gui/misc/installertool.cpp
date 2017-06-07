#include "installertool.h"

#include <misc/iricrootpath.h>

#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QString>

InstallerTool::InstallerTool()
{}

InstallerTool::~InstallerTool()
{}

void InstallerTool::openMaintainanceDialog()
{
	QProcess::startDetached(QString("\"%1\"").arg(installerFileName()));
}

QString InstallerTool::installerFileName()
{
	QDir guiDir(iRICRootPath::get());
	// move to installer folder.
	guiDir.cdUp();
	guiDir.cdUp();
	return guiDir.absoluteFilePath("maintenancetool.exe");
}
