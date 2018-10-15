#include "installertool.h"
#include "../main/iricmainwindow.h"

#include <misc/iricrootpath.h>

#include <QAbstractButton>
#include <QDir>
#include <QMessageBox>
#include <QProcess>

InstallerTool::InstallerTool()
{}

InstallerTool::~InstallerTool()
{}

void InstallerTool::openMaintainanceDialog(iRICMainWindow* w)
{
	QMessageBox warningDialog(QMessageBox::Information, tr("Warning"),
														tr("When you update iRIC, you need to close iRIC before launching iRIC Maintainance.\n"
														"When you have iRIC windows other than this, close them manually."),
														QMessageBox::Ok | QMessageBox::Cancel, w);
	warningDialog.button(QMessageBox::Ok)->setText(tr("Close iRIC and launch iRIC Maintainance"));
	int ret = warningDialog.exec();
	if (ret == QMessageBox::Cancel) {return;}

	bool closed = w->closeProject();
	if (! closed) {return;}

	launchMaintainance();

	w->close();
}

void InstallerTool::launchMaintainance()
{
	QProcess::startDetached(QString("\"%1\" --updater").arg(installerFileName()));
}

QString InstallerTool::installerFileName()
{
	QDir guiDir(iRICRootPath::get());
	// move to installer folder.
	guiDir.cdUp();
	guiDir.cdUp();
	return guiDir.absoluteFilePath("maintenancetool.exe");
}
