#ifndef WAITDIALOGEXECUTERWATCHER_H
#define WAITDIALOGEXECUTERWATCHER_H

#include "../guibase_global.h"
#include "executerwatcher.h"

#include <QObject>

class WaitDialog;

class GUIBASEDLL_EXPORT WaitDialogExecuterWatcher : public QObject, public ExecuterWatcher
{
	Q_OBJECT

public:
	WaitDialogExecuterWatcher(ExecuterI* executer, QWidget* widget);
	~WaitDialogExecuterWatcher();

	void setMessage(const QString& message) override;

	void setInitialWaitTime(int waitMSec, int progress);
	void setProgressSpeed(int speedParam, int waitMsec);

	void execute() override;

private slots:
	void cancel();

private:
	int m_initialWaitMsec;
	int m_initialProgress;
	int m_waitMsec;

	WaitDialog* m_waitDialog;
};

#endif // WAITDIALOGEXECUTERWATCHER_H
