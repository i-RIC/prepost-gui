#ifndef SPRINGSOLVERTHREAD_H
#define SPRINGSOLVERTHREAD_H

#include <QThread>

class SpringSolverThread : public QThread
{
	Q_OBJECT

public:
	SpringSolverThread(QObject* parent = nullptr);

	void setGrid(std::vector<double>* x, std::vector<double>* y, int imax, int jmax);
	void setCondition(double eps, int maxIter);

	int progress() const;

public slots:
	void cancel();

private:
	void run() override;

	std::vector<double>* m_x;
	std::vector<double>* m_y;
	int m_iMax;
	int m_jMax;

	double m_eps;
	double m_maxIteration;

	int m_progress;
	bool m_canceled;
};

#endif // SPRINGSOLVERTHREAD_H
