#ifndef GRIDCREATINGCONDITIONLAPLACE_BUILDSUBGRIDPOINTSTHREAD_H
#define GRIDCREATINGCONDITIONLAPLACE_BUILDSUBGRIDPOINTSTHREAD_H

#include "../gridcreatingconditionlaplace.h"

#include <QMutex>
#include <QThread>

class GridCreatingConditionLaplace::BuildSubGridPointsThread : public QThread
{
	Q_OBJECT

public:
	BuildSubGridPointsThread(GridCreatingConditionLaplace::Impl* impl);
	~BuildSubGridPointsThread();

	void restartBuild(int i, int j);
	void finish();

signals:
	void buildFinished(int i, int j);

private:
	void run() override;

	bool m_restart;
	bool m_finish;
	bool m_finished;

	int m_i;
	int m_j;

	QMutex m_mutex;

	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_BUILDSUBGRIDPOINTSTHREAD_H
