#include "installertool.h"

#include <misc/iricrootpath.h>

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QString>

InstallerTool::InstallerTool()
{}

InstallerTool::~InstallerTool()
{}

void InstallerTool::openMaintainanceDialog(QWidget* w)
{
	QMessageBox::warning(w, tr("Warning"), tr("When you want to update iRIC GUI, please close all iRIC GUI windows before updating, so that iRIC Maintainance can overwrite it."));

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
