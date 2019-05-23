#include "gridcreatingconditionlaplace_buildsubgridpointsthread.h"
#include "gridcreatingconditionlaplace_impl.h"

#include <QMutexLocker>

GridCreatingConditionLaplace::BuildSubGridPointsThread::BuildSubGridPointsThread(GridCreatingConditionLaplace::Impl* impl) :
	QThread {},
	m_restart {false},
	m_finish {false},
	m_finished {false},
	m_i {0},
	m_j {0},
	m_impl {impl}
{}

GridCreatingConditionLaplace::BuildSubGridPointsThread::~BuildSubGridPointsThread()
{}

void GridCreatingConditionLaplace::BuildSubGridPointsThread::restartBuild(int i, int j)
{
	QMutexLocker locker(&m_mutex);

	m_i = i;
	m_j = j;
	m_restart = true;
}

void GridCreatingConditionLaplace::BuildSubGridPointsThread::finish()
{
	m_mutex.lock();
	m_finish = true;
	m_mutex.unlock();

	while (! m_finished) {
		msleep(10);
	}
}

void GridCreatingConditionLaplace::BuildSubGridPointsThread::run()
{
	forever {
		if (m_finish) {
			m_finished = true;
			return;
		}

		if (! m_restart) {
			msleep(10);
			continue;
		}

		m_mutex.lock();
		m_restart = false;
		m_mutex.unlock();

		auto edgeLineStreamWise1 = m_impl->edgeLineStreamWiseForDivisionPreview(m_i, m_j).polyLine();
		auto edgeLineStreamWise2 = m_impl->edgeLineStreamWiseForDivisionPreview(m_i, m_j + 1).polyLine();
		auto edgeLineCrossSection1 = m_impl->edgeLineCrossSectionForDivisionPreview(m_i, m_j).polyLine();
		auto edgeLineCrossSection2 = m_impl->edgeLineCrossSectionForDivisionPreview(m_i + 1, m_j).polyLine();

		auto points = Impl::buildSubGrid(edgeLineStreamWise1, edgeLineStreamWise2,
																		 edgeLineCrossSection1, edgeLineCrossSection2,
																		 m_impl->subRegionPoissonParameter(m_i, m_j));

		if (m_finish) {
			m_finished = true;
			return;
		}
		if (m_restart) {
			continue;
		}

		m_mutex.lock();

		m_restart = false;
		m_impl->m_previewGridPoints = points;

		emit buildFinished(m_i, m_j);

		m_mutex.unlock();
	}
}
