#include "gridcreatingconditionlaplace_commonratiooptimizethread.h"
#include "gridcreatingconditionlaplace_impl.h"

#include <misc/anms.h>

#include <QMutexLocker>

GridCreatingConditionLaplace::CommonRatioOptimizeThread::CommonRatioOptimizeThread(GridCreatingConditionLaplace::Impl* impl) :
	QThread {},
	m_restart {false},
	m_finish {false},
	m_finished {false},
	m_impl {impl}
{}

GridCreatingConditionLaplace::CommonRatioOptimizeThread::~CommonRatioOptimizeThread()
{}

void GridCreatingConditionLaplace::CommonRatioOptimizeThread::restartOpt()
{
	QMutexLocker locker(&m_mutex);

	std::vector<OptimizeParam> streamWiseParams;

	// optimize streamwise ratios
	for (int j = 0; j < m_impl->m_ctrlPointCountJ; ++j) {
		OptimizeParam p;

		p.divNums = m_impl->m_divCountsStreamWise;
		for (int i = 0; i < m_impl->m_ctrlPointCountI - 1; ++i) {
			auto lengths = Impl::buildLengths(m_impl->edgeLineStreamWiseForEdgeSelection(i, j).polyLine());
			p.lengths.push_back(lengths.back());
			p.divModes.push_back(m_impl->divModeStreamWise(i, j));
			p.commonRatios.push_back(m_impl->divCommonRatioStreamWise(i, j));
		}
		streamWiseParams.push_back(p);
	}

	std::vector<OptimizeParam> crossSectionParams;

	// optimize crosssection ratios
	for (int i = 0; i < m_impl->m_ctrlPointCountI; ++i) {
		OptimizeParam p;

		p.divNums = m_impl->m_divCountsCrossSection;
		for (int j = 0; j < m_impl->m_ctrlPointCountJ - 1; ++j) {
			auto lengths = Impl::buildLengths(m_impl->edgeLineCrossSectionForEdgeSelection(i, j).polyLine());
			p.lengths.push_back(lengths.back());
			p.divModes.push_back(m_impl->divModeCrossSection(i, j));
			p.commonRatios.push_back(m_impl->divCommonRatioCrossSection(i, j));
		}
		crossSectionParams.push_back(p);
	}

	m_streamWiseParams = streamWiseParams;
	m_crossSectionParams = crossSectionParams;

	m_restart = true;
}

void GridCreatingConditionLaplace::CommonRatioOptimizeThread::finish()
{
	m_mutex.lock();
	m_finish = true;
	m_mutex.unlock();

	while (! m_finished) {
		msleep(10);
	}
}

void GridCreatingConditionLaplace::CommonRatioOptimizeThread::run()
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

		std::vector<double> streamWiseCommonRatios;
		std::vector<double> crossSectionCommonRatios;

		streamWiseCommonRatios.assign((m_impl->m_ctrlPointCountI - 1) * m_impl->m_ctrlPointCountJ, 1.0);
		crossSectionCommonRatios.assign(m_impl->m_ctrlPointCountI * (m_impl->m_ctrlPointCountJ - 1), 1.0);

		auto streamWiseCount = m_streamWiseParams.size();
		for (int j = 0; j < streamWiseCount; ++j) {
			if (m_finish) {
				m_finished = true;
				return;
			}

			if (m_restart) {
				break;
			}

			auto ratios = optimizedCommonRatios(m_streamWiseParams.at(j));
			for (int i = 0; i < ratios.size(); ++i) {
				streamWiseCommonRatios[i + j * (m_impl->m_ctrlPointCountI - 1)] = ratios[i];
			}
		}
		auto crossSectionCount = m_crossSectionParams.size();
		for (int i = 0; i < crossSectionCount; ++i) {
			if (m_finish) {
				m_finished = true;
				return;
			}

			if (m_restart) {
				break;
			}

			auto ratios = optimizedCommonRatios(m_crossSectionParams.at(i));
			for (int j = 0; j < ratios.size(); ++j) {
				crossSectionCommonRatios[i + j * m_impl->m_ctrlPointCountI] = ratios[j];
			}
		}

		if (m_finish) {
			m_finished = true;
			return;
		}
		if (m_restart) {
			continue;
		}

		m_mutex.lock();

		m_restart = false;
		m_impl->m_tmpDivCommonRatiosStreamWise = streamWiseCommonRatios;
		m_impl->m_tmpDivCommonRatiosCrossSection = crossSectionCommonRatios;

		emit optimizeFinished();

		m_mutex.unlock();
	}
}

std::vector<double> GridCreatingConditionLaplace::CommonRatioOptimizeThread::optimizedCommonRatios(OptimizeParam& p)
{
	std::vector<double> ratios;
	for (auto mode : p.divModes) {
		if (mode == DivisionMode::Auto) {
			ratios.push_back(1);
		}
	}
	if (ratios.size() == 0) {
		return p.commonRatios;
	}

	ANMS anms;
	anms.minimize(&edgeOptimizeFuncEx, &p, ratios, 1000, 1.0e-7, 1.0e-5);

	int idx = 0;
	std::vector<double> ret;
	for (int i = 0; i < p.divModes.size(); ++i) {
		if (p.divModes[i]	== DivisionMode::Auto) {
			ret.push_back(anms.xout.at(idx));
			++ idx;
		} else {
			ret.push_back(p.commonRatios.at(i));
		}
	}
	return ret;
}

double GridCreatingConditionLaplace::CommonRatioOptimizeThread::edgeOptimizeFunc(const std::vector<double>& lengths, const std::vector<int>& divNums, const std::vector<DivisionMode>& divModes, const std::vector<double>& commonRatios)
{
	std::vector<std::pair<double, double> > firstAndLastEdgeLengths;

	double r_weight = 0.5;
	double ret = 0;

	for (int i = 0; i < lengths.size(); ++i) {
		auto len = lengths.at(i);
		auto divNum = divNums.at(i);
		auto divMode = divModes.at(i);
		auto commonRatio = commonRatios.at(i);

		double r = commonRatio;
		if (r < 1) {r = 1 / r;}
		if (divMode == DivisionMode::Auto) {
			ret += r_weight * r;
		}
		std::vector<double> lengthsForPoints;
		if (divMode == DivisionMode::Auto) {
			lengthsForPoints = Impl::buildLengthsForPoints(len, divNum, DivisionMode::Geometric, commonRatio);
		} else {
			lengthsForPoints = Impl::buildLengthsForPoints(len, divNum, divMode, commonRatio);
		}
		auto first = *(lengthsForPoints.begin() + 1) - *(lengthsForPoints.begin());
		auto last = *(lengthsForPoints.begin() + lengthsForPoints.size() - 1) - *(lengthsForPoints.begin() + lengthsForPoints.size() - 2);
		firstAndLastEdgeLengths.push_back(std::make_pair(first, last));
	}

	for (int i = 0; i < firstAndLastEdgeLengths.size() - 1; ++i) {
		auto& pair1 = firstAndLastEdgeLengths[i];
		auto& pair2 = firstAndLastEdgeLengths[i + 1];

		double ratio = pair1.second / pair2.first;
		if (ratio < 1) {ratio = 1 / ratio;}
		ret += ratio;
	}
	return ret;
}

double GridCreatingConditionLaplace::CommonRatioOptimizeThread::edgeOptimizeFuncEx(int n, const double* x0, void* ex)
{
	auto params = reinterpret_cast<OptimizeParam*> (ex);
	std::vector<double> ratios = params->commonRatios;
	int idx = 0;
	for (int i = 0; i < params->divModes.size(); ++i) {
		if (params->divModes.at(i) == GridCreatingConditionLaplace::DivisionMode::Auto) {
			ratios[i] = *(x0 + idx);
			++ idx;
		}
	}
	return edgeOptimizeFunc(params->lengths, params->divNums, params->divModes, ratios);
}
