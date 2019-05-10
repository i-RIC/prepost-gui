#ifndef GRIDCREATINGCONDITIONLAPLACE_COMMONRATIOOPTIMIZETHREAD_H
#define GRIDCREATINGCONDITIONLAPLACE_COMMONRATIOOPTIMIZETHREAD_H

#include "../gridcreatingconditionlaplace.h"

#include <QMutex>
#include <QThread>

class GridCreatingConditionLaplace::CommonRatioOptimizeThread : public QThread
{
	Q_OBJECT

public:
	struct OptimizeParam {
		std::vector<double> lengths;
		std::vector<int> divNums;
		std::vector<GridCreatingConditionLaplace::DivisionMode> divModes;
		std::vector<double> commonRatios;
	};

	CommonRatioOptimizeThread(GridCreatingConditionLaplace::Impl* impl);
	~CommonRatioOptimizeThread();

	void restartOpt();
	void finish();

signals:
	void optimizeFinished();

protected:
	void run() override;

private:
	static std::vector<double> optimizedCommonRatios(OptimizeParam& p);
	static double edgeOptimizeFunc(const std::vector<double>& lengths, const std::vector<int>& divNums, const std::vector<DivisionMode>& divModes, const std::vector<double>& commonRatios);
	static double edgeOptimizeFuncEx(int n, const double* x0, void* ex);

	bool m_restart;
	bool m_finish;
	bool m_finished;

	std::vector<OptimizeParam> m_streamWiseParams;
	std::vector<OptimizeParam> m_crossSectionParams;

	QMutex m_mutex;

	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_COMMONRATIOOPTIMIZETHREAD_H
