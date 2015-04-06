#include "triangleexecutethread.h"
#include "triangle.h"

#include <QProcess>

#define REAL double
#define VOID void

TriangleExecuteThread::TriangleExecuteThread(QObject *parent) :
	QThread(parent)
{
	m_in = 0;
	m_out = 0;
	m_args = 0;
}

void TriangleExecuteThread::run()
{
	triangulate(m_args, m_in, m_out, 0);

	if (m_fileArgs != ""){
		QProcess* process = new QProcess(this);
		QStringList args;
		args << m_fileArgs << m_fileName;
		process->setWorkingDirectory(m_workFolder);
		process->start(m_exeFile, args);
		process->waitForFinished(-1);
		delete process;
	}
	exit();
}
