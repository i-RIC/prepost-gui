#define REAL double
#define VOID void

#include "triangle.h"
#include "triangleexecutethread.h"
#include "private/triangleexecutethread_impl.h"

#include <misc/stringtool.h>

#include <QProcess>

TriangleExecuteThread::TriangleExecuteThread(QObject* parent) :
	QThread {parent},
	impl {new Impl{}}
{}

TriangleExecuteThread::~TriangleExecuteThread()
{
	delete impl;
}

void TriangleExecuteThread::setIOs(triangulateio* in, triangulateio* out)
{
	impl->m_in = in;
	impl->m_out = out;
}

void TriangleExecuteThread::setArgs(const QString& args)
{
	impl->m_args = iRIC::toStr(args);
}

void TriangleExecuteThread::setFileOutputSetting(const QString& exeFile, const QString& args, const QString& filename, const QString& workFolder)
{
	impl->m_exeFile = exeFile;
	impl->m_fileArgs = args;
	impl->m_fileName = filename;
	impl->m_workFolder = workFolder;
}

void TriangleExecuteThread::run()
{
	triangulate(const_cast<char*>(impl->m_args.c_str()), impl->m_in, impl->m_out, nullptr);

	if (impl->m_fileArgs != "") {
		QProcess process(this);
		QStringList args;
		args << impl->m_fileArgs << impl->m_fileName;
		process.setWorkingDirectory(impl->m_workFolder);
		process.start(impl->m_exeFile, args);
		process.waitForFinished(-1);
	}
	exit();
}
