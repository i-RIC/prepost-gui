#include "flushrequester.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QThread>

void FlushRequester::requestFlush(const QString& projectFolder, int idx, int timeOutMSec)
{
	QString fname = flushFileName(projectFolder);
	createFlushFile(fname, idx);
	waitUntilFlushFileRemoved(fname, timeOutMSec);
}

void FlushRequester::createFlushFile(const QString& filename, int idx)
{
	QFile f(filename);
	f.open(QFile::WriteOnly);
	QTextStream stream(&f);

	stream << idx;

	f.close();
}

bool FlushRequester::flushIsCopying(const QString& filename)
{
	QFile f(filename);
	if (! f.exists()) {
		return false;
	}

	f.open(QFile::ReadOnly);
	QString content = f.readAll().trimmed();
	return (content == "copying");
}

void FlushRequester::waitUntilFlushFileRemoved(const QString& filename, int timeOutMSec)
{
	int waitTimeMSec = 10;

	while (timeOutMSec > 0 && flushIsCopying(filename)) {
		QThread::msleep(waitTimeMSec);
		timeOutMSec -= waitTimeMSec;
		qApp->processEvents();
	}
}

QString FlushRequester::flushFileName(const QString& projectFolder)
{
	QDir dir(projectFolder);
	return dir.absoluteFilePath(".flush");
}

FlushRequester::FlushRequester()
{}
