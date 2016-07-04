#include "processexecuter.h"

#include <QProcess>

ProcessExecuter::ProcessExecuter(QProcess* process) :
	m_process {process},
	m_isCanceled {false},
	m_isFinished {false},
	m_exitCode {-1},
	m_exitStatus {QProcess::NormalExit}
{
	connect(m_process, SIGNAL(finished(int)), this, SLOT(handleFinished()));
}

ProcessExecuter::~ProcessExecuter()
{
	delete m_process;
}

void ProcessExecuter::start()
{
	m_process->waitForStarted();
}

void ProcessExecuter::wait(int msec)
{
	m_process->waitForFinished(msec);
}

void ProcessExecuter::cancel()
{
	m_isCanceled = true;
	m_process->kill();
}

bool ProcessExecuter::isFinished() const
{
	return m_isFinished;
}

bool ProcessExecuter::isCanceled() const
{
	return m_isCanceled;
}

int ProcessExecuter::exitCode() const
{
	return m_exitCode;
}

QProcess::ExitStatus ProcessExecuter::exitStatus() const
{
	return m_exitStatus;
}

QProcess* ProcessExecuter::process() const
{
	return m_process;
}

void ProcessExecuter::handleFinished()
{
	m_isFinished = true;
}
