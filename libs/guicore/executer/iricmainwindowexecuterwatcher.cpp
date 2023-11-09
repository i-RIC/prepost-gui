#include "iricmainwindowexecuterwatcher.h"
#include "../base/iricmainwindowi.h"

iRICMainWindowExecuterWatcher::iRICMainWindowExecuterWatcher(ExecuterI* executer, iRICMainWindowI* w) :
	WaitDialogExecuterWatcher(executer, w),
	m_mainWindow {w}
{}

void iRICMainWindowExecuterWatcher::execute()
{
	m_mainWindow->enterModelessDialogMode();

	WaitDialogExecuterWatcher::execute();

	m_mainWindow->exitModelessDialogMode();
}
