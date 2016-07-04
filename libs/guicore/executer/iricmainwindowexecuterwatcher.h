#ifndef IRICMAINWINDOWEXECUTERWATCHER_H
#define IRICMAINWINDOWEXECUTERWATCHER_H

#include "../guicore_global.h"

#include <guibase/executer/waitdialogexecuterwatcher.h>

class iRICMainWindowInterface;

class GUICOREDLL_EXPORT iRICMainWindowExecuterWatcher : public WaitDialogExecuterWatcher
{
public:
	iRICMainWindowExecuterWatcher(ExecuterI* executer, iRICMainWindowInterface* w);

	void execute() override;

private:
	iRICMainWindowInterface* m_mainWindow;
};

#endif // IRICMAINWINDOWEXECUTERWATCHER_H
