#define REAL double
#define VOID void

#include "triangle.h"
#include "triangleexecutethread.h"
#include "private/triangleexecutethread_impl.h"

#include <QProcess>

TriangleExecuteThread::TriangleExecuteThread(QObject* parent) :
	QThread {parent},
	m_impl {new Impl{}}
{}

TriangleExecuteThread::~TriangleExecuteThread()
{
	delete m_impl;
}

void TriangleExecuteThread::setIOs(triangulateio* in, triangulateio* out)
{
	m_impl->m_in = in;
	m_impl->m_out = out;
}

void TriangleExecuteThread::setArgs(char* args)
{
	m_impl->m_args = args;
}

void TriangleExecuteThread::setFileOutputSetting(const QString& exeFile, const QString& args, const QString& filename, const QString& workFolder)
{
	m_impl->m_exeFile = exeFile;
	m_impl->m_fileArgs = args;
	m_impl->m_fileName = filename;
	m_impl->m_workFolder = workFolder;
}

void TriangleExecuteThread::run()
{
	triangulate(m_impl->m_args, m_impl->m_in, m_impl->m_out, nullptr);

	if (m_impl->m_fileArgs != "") {
		QProcess process(this);
		QStringList args;
		args << m_impl->m_fileArgs << m_impl->m_fileName;
		process.setWorkingDirectory(m_impl->m_workFolder);
		process.start(m_impl->m_exeFile, args);
		process.waitForFinished(-1);
	}
	exit();
}
