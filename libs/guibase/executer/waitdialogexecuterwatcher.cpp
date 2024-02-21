#include "waitdialogexecuterwatcher.h"
#include "../widget/waitdialog.h"
#include "executeri.h"

#include <QApplication>

WaitDialogExecuterWatcher::WaitDialogExecuterWatcher(ExecuterI* executer, QWidget* widget) :
	ExecuterWatcher(executer),
	m_initialWaitMsec {3000},
	m_initialProgress {0},
	m_waitMsec {200},
	m_waitDialog {new WaitDialog(widget)}
{
	m_waitDialog->setMessage(tr("Please wait..."));
	m_waitDialog->showProgressBar();
	m_waitDialog->setRange(0, 100);
	m_waitDialog->setUnknownLimitMode(300);
}

WaitDialogExecuterWatcher::~WaitDialogExecuterWatcher()
{
	delete m_waitDialog;
}

void WaitDialogExecuterWatcher::setMessage(const QString& message)
{
	m_waitDialog->setMessage(message);
}

void WaitDialogExecuterWatcher::setInitialWaitTime(int waitMSec, int progress)
{
	m_initialWaitMsec = waitMSec;
	m_initialProgress = progress;
}

void WaitDialogExecuterWatcher::setProgressSpeed(int speedParam, int waitMsec)
{
	m_waitMsec = waitMsec;
	m_waitDialog->setUnknownLimitMode(speedParam);
}

void WaitDialogExecuterWatcher::execute()
{
	auto e = executer();
	e->start();

	e->wait(m_initialWaitMsec);
	qApp->processEvents();

	if (e->isFinished() || e->isCanceled()) {
		return;
	}
	connect(m_waitDialog, &WaitDialog::canceled, this, &WaitDialogExecuterWatcher::cancel);
	int progress = m_initialProgress;

	m_waitDialog->setProgress(progress);
	m_waitDialog->show();

	while (! e->isFinished() && ! e->isCanceled()) {
		e->wait(m_waitMsec);
		m_waitDialog->setProgress(++ progress);
		qApp->processEvents();
	}
	m_waitDialog->hide();
}

void WaitDialogExecuterWatcher::cancel()
{
	executer()->cancel();
}
