#include "flushrequester.h"
#include "private/cancelhandler.h"

#include <guibase/widget/waitdialog.h>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QThread>
#include <QWidget>

bool FlushRequester::requestFlush(const QString& projectFolder, int idx, QWidget* parent)
{
	QString fname = flushFileName(projectFolder);
	createFlushFile(fname, idx);
	return waitUntilFlushFileRemoved(fname, parent);
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
	return true;
}

bool FlushRequester::waitUntilFlushFileRemoved(const QString& filename, QWidget *parent)
{
	int waitTimeMSec = 10;

	WaitDialog dialog(parent);
	CancelHandler handler;

	connect(&dialog, SIGNAL(canceled()), &handler, SLOT(cancel()));
	dialog.setMessage(tr("Waiting for solver to copy CGNS file..."));
	dialog.show();

	while (! handler.isCanceled() && flushIsCopying(filename)) {
		QThread::msleep(waitTimeMSec);
		qApp->processEvents();
	}
	return ! handler.isCanceled();
}

QString FlushRequester::flushFileName(const QString& projectFolder)
{
	QDir dir(projectFolder);
	return dir.absoluteFilePath(".flush");
}

FlushRequester::FlushRequester(QObject* parent) :
	QObject(parent)
{}
