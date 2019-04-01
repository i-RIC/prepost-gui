#include "gridcreatingconditionlaplace_edgeeditcoordinatescommand.h"
#include "gridcreatingconditionlaplace_impl.h"

GridCreatingConditionLaplace::EdgeEditCoordinatesCommand::EdgeEditCoordinatesCommand(bool streamWise, int i, int j, const std::vector<QPointF>& coords, GridCreatingConditionLaplace::Impl* impl) :
	m_streamWise {streamWise},
	m_i {i},
	m_j {j},
	m_newCoords (coords),
	m_impl {impl}
{
	if (streamWise) {
		m_oldCoords.push_back(impl->ctrlPoint(i, j));
		for (auto p : m_impl->edgeLineStreamWise(i, j).polyLine()) {
			m_oldCoords.push_back(p);
		}
		m_oldCoords.push_back(impl->ctrlPoint(i + 1, j));
	} else {
		m_oldCoords.push_back(impl->ctrlPoint(i, j));
		for (auto p : m_impl->edgeLineCrossSection(i, j).polyLine()) {
			m_oldCoords.push_back(p);
		}
		m_oldCoords.push_back(impl->ctrlPoint(i, j + 1));
	}
}

void GridCreatingConditionLaplace::EdgeEditCoordinatesCommand::redo()
{
	apply(m_newCoords);
}

void GridCreatingConditionLaplace::EdgeEditCoordinatesCommand::undo()
{
	apply(m_oldCoords);
}

void GridCreatingConditionLaplace::EdgeEditCoordinatesCommand::apply(const std::vector<QPointF>& points)
{
	auto firstp = points.front();
	auto lastp = points.back();
	auto otherPoints = points;

	otherPoints.pop_back();
	otherPoints.erase(otherPoints.begin());

	if (m_streamWise) {
		m_impl->ctrlPoint(m_i, m_j) = firstp;
		m_impl->ctrlPoint(m_i + 1, m_j) = lastp;
		m_impl->edgeLineStreamWise(m_i, m_j).setPolyLine(otherPoints);
	} else {
		m_impl->ctrlPoint(m_i, m_j) = firstp;
		m_impl->ctrlPoint(m_i, m_j + 1) = lastp;
		m_impl->edgeLineCrossSection(m_i, m_j).setPolyLine(otherPoints);
	}
}
