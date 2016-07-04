#ifndef PROCESSEXECUTER_H
#define PROCESSEXECUTER_H

#include "../guibase_global.h"
#include "executeri.h"

#include <QObject>
#include <QProcess>

class QProcess;

class GUIBASEDLL_EXPORT ProcessExecuter : public QObject, public ExecuterI
{
	Q_OBJECT

public:
	ProcessExecuter(QProcess* process);
	virtual ~ProcessExecuter();

	void start() override;
	void wait(int msec) override;
	void cancel() override;

	bool isFinished() const override;
	bool isCanceled() const override;

	int exitCode() const;
	QProcess::ExitStatus exitStatus() const;

protected:
	QProcess* process() const;

private slots:
	void handleFinished();

private:
	QProcess* m_process;

	bool m_isCanceled;

	bool m_isFinished;
	int m_exitCode;
	QProcess::ExitStatus m_exitStatus;
};

#endif // PROCESSEXECUTER_H
