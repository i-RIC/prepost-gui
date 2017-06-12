#include "installertool.h"
#include "periodicalupdatechecker.h"

#include <QAbstractButton>
#include <QDateTime>
#include <QMessageBox>
#include <QSettings>

const int PeriodicalUpdateChecker::DEFAULT_INTERVAL_DAYS = 28;

bool PeriodicalUpdateChecker::checkUpdate()
{
	QSettings settings;
	bool checkPeriodically = settings.value("general/periodicalUpdate", true).toBool();
	int checkInterval = settings.value("general/periodicalUpdateInterval", PeriodicalUpdateChecker::DEFAULT_INTERVAL_DAYS).toInt();
	int lastCheck = settings.value("general/periodicalLastUpdate", 0).toUInt();

	if (checkPeriodically == false) {return false;}

	bool check = false;
	if (lastCheck == 0) {
		check = true;
	} else {
		QDateTime nowTime = QDateTime::currentDateTime();
		QDateTime lastCheckTime = QDateTime::fromTime_t(lastCheck);
		QDateTime nextCheckTime = lastCheckTime.addDays(checkInterval);
		check = (nowTime > nextCheckTime);
	}

	if (! check) {return false;}

	QDateTime nowTime = QDateTime::currentDateTime();
	settings.setValue("general/periodicalLastUpdate", nowTime.toTime_t());

	QMessageBox dialog(QMessageBox::Information, tr("Check for Update"),
										 tr("%1 days have passed after launching iRIC Maintainance for the last time.\n"
												"Do you want to launch iRIC Maintainance?").arg(checkInterval), QMessageBox::Ok | QMessageBox::Cancel);
	dialog.button(QMessageBox::Ok)->setText(tr("Launch iRIC Maintainance"));
	dialog.setWindowIcon(QIcon(":/images/iconiRIC.png"));
	int ret = dialog.exec();

	if (ret == QMessageBox::Cancel) {return false;}

	InstallerTool::launchMaintainance();
	return true;
}

PeriodicalUpdateChecker::PeriodicalUpdateChecker()
{}
