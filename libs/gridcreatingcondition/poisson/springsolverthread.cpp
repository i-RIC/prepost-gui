#include "springsolver.h"
#include "springsolverthread.h"

SpringSolverThread::SpringSolverThread(QObject* parent) :
	QThread {parent},
	m_progress {0},
	m_canceled {false}
{}

void SpringSolverThread::setGrid(std::vector<double>* x, std::vector<double>* y, int imax, int jmax)
{
	m_x = x;
	m_y = y;
	m_iMax = imax;
	m_jMax = jmax;
}

void SpringSolverThread::setCondition(double eps, int maxIter)
{
	m_eps = eps;
	m_maxIteration = maxIter;
}

int SpringSolverThread::progress() const
{
	return m_progress;
}

void SpringSolverThread::cancel()
{
	m_canceled = true;
}

void SpringSolverThread::run()
{
	SpringSolver::solve(m_x, m_y, m_iMax, m_jMax, m_eps, m_maxIteration, &m_progress, &m_canceled);
	exit();
}
