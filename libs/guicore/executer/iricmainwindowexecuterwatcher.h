#ifndef IRICMAINWINDOWEXECUTERWATCHER_H
#define IRICMAINWINDOWEXECUTERWATCHER_H

#include "../guicore_global.h"

#include <guibase/executer/waitdialogexecuterwatcher.h>

class iRICMainWindowI;

class GUICOREDLL_EXPORT iRICMainWindowExecuterWatcher : public WaitDialogExecuterWatcher
{
public:
	iRICMainWindowExecuterWatcher(ExecuterI* executer, iRICMainWindowI* w);

	void execute() override;

private:
	iRICMainWindowI* m_mainWindow;
};

#endif // IRICMAINWINDOWEXECUTERWATCHER_H
