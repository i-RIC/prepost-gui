#include "executeri.h"
#include "silentexecuterwatcher.h"

#include <QCoreApplication>

SilentExecuterWatcher::SilentExecuterWatcher(ExecuterI* executer) :
	ExecuterWatcher(executer)
{}

void SilentExecuterWatcher::setCheckInterval(int msec)
{
	m_checkInterval = msec;
}

void SilentExecuterWatcher::execute()
{
	auto e = executer();
	e->start();

	while (! e->isFinished()) {
		e->wait(m_checkInterval);
		qApp->processEvents();
	}
}
