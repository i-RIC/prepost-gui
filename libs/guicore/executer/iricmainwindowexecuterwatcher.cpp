#include "iricmainwindowexecuterwatcher.h"
#include "../base/iricmainwindowinterface.h"

iRICMainWindowExecuterWatcher::iRICMainWindowExecuterWatcher(ExecuterI* executer, iRICMainWindowInterface* w) :
	WaitDialogExecuterWatcher(executer, w),
	m_mainWindow {w}
{}

void iRICMainWindowExecuterWatcher::execute()
{
	m_mainWindow->enterModelessDialogMode();

	WaitDialogExecuterWatcher::execute();

	m_mainWindow->exitModelessDialogMode();
}
