#ifndef THREADEXECUTER_H
#define THREADEXECUTER_H

#include "../guibase_global.h"
#include "executeri.h"

#include <QObject>

class QThread;

class GUIBASEDLL_EXPORT ThreadExecuter : public QObject, public ExecuterI
{
	Q_OBJECT

public:
	ThreadExecuter(QThread* thread);
	virtual ~ThreadExecuter();

	void start() override;
	void wait(int msec) override;
	void cancel() override;

	bool isFinished() const override;
	bool isCanceled() const override;

protected:
	QThread* thread() const;

private slots:
	void handleFinished();

private:
	QThread* m_thread;

	bool m_isCanceled;
	bool m_isFinished;
};

#endif // THREADEXECUTER_H
