#ifndef SILENTEXECUTERWATCHER_H
#define SILENTEXECUTERWATCHER_H

#include "../guibase_global.h"
#include "executerwatcher.h"

class GUIBASEDLL_EXPORT SilentExecuterWatcher : public ExecuterWatcher
{
public:
	SilentExecuterWatcher(ExecuterI* executer);
	void setCheckInterval(int msec);

	void execute() override;

private:
	int m_checkInterval;
};

#endif // SILENTEXECUTERWATCHER_H
