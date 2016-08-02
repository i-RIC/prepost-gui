#ifndef IRICMAINWINDOW_SNAPSHOTSAVER_H
#define IRICMAINWINDOW_SNAPSHOTSAVER_H

#include "../iricmainwindow.h"

class SnapshotEnabledWindowInterface;

class iRICMainWindow::SnapshotSaver
{
public:
	SnapshotSaver(iRICMainWindow* mainWindow);

	void save(SnapshotEnabledWindowInterface* enabledWindow);

private:
	iRICMainWindow* m_mainWindow;
};

#endif // IRICMAINWINDOW_SNAPSHOTSAVER_H
