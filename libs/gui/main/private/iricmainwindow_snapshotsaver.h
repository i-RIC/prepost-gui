#ifndef IRICMAINWINDOW_SNAPSHOTSAVER_H
#define IRICMAINWINDOW_SNAPSHOTSAVER_H

#include "../iricmainwindow.h"

class QMainWindowWithSnapshot;

class iRICMainWindow::SnapshotSaver
{
public:
	SnapshotSaver(iRICMainWindow* mainWindow);

	void save(QMainWindowWithSnapshot* enabledWindow);

private:
	iRICMainWindow* m_mainWindow;
};

#endif // IRICMAINWINDOW_SNAPSHOTSAVER_H
