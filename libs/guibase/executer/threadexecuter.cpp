#include "threadexecuter.h"

#include <QThread>

ThreadExecuter::ThreadExecuter(QThread* thread) :
	m_thread {thread},
	m_isCanceled {false},
	m_isFinished {false}
{
	connect(m_thread, SIGNAL(finished()), this, SLOT(handleFinished()));
}

ThreadExecuter::~ThreadExecuter()
{
	delete m_thread;
}

void ThreadExecuter::start()
{
	m_thread->start();
}

void ThreadExecuter::wait(int msec)
{
	m_thread->wait(msec);
}

void ThreadExecuter::cancel()
{
	m_isCanceled = true;
	m_thread->terminate();
}

bool ThreadExecuter::isFinished() const
{
	return m_isFinished;
}

bool ThreadExecuter::isCanceled() const
{
	return m_isCanceled;
}

QThread* ThreadExecuter::thread() const
{
	return m_thread;
}

void ThreadExecuter::handleFinished()
{
	m_isFinished = true;
}
