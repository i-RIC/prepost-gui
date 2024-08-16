#include "gridcreatingconditionlaplace_edgesetinterpolationmodecommand.h"

GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand::EdgeSetInterpolationModeCommand(Impl::EdgeType edgeType, std::vector<int> edgeIds, InterpolationType type, GridCreatingConditionLaplace::Impl* impl) :
	m_newTypes {},
	m_oldTypes {},
	m_edgeType {edgeType},
	m_edgeIds {edgeIds},
	m_impl {impl}
{
	m_newTypes.assign(m_edgeIds.size(), type);

	for (auto id : m_edgeIds) {
		m_oldTypes.push_back(targetType(id));
	}
}

void GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand::redo()
{
	for (unsigned int i = 0; i < m_edgeIds.size(); ++i) {
		auto id = m_edgeIds[i];
		targetType(id) = m_newTypes[i];
	}
}

void GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand::undo()
{
	for (unsigned int i = 0; i < m_edgeIds.size(); ++i) {
		auto id = m_edgeIds[i];
		targetType(id) = m_oldTypes[i];
	}
}

GridCreatingConditionLaplace::InterpolationType& GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand::targetType(int id)
{
	if (m_edgeType == Impl::EdgeType::StreamWise) {
		return m_impl->m_edgeInterpolationStreamWise[id];
	} else {
		return m_impl->m_edgeInterpolationCrossSection[id];
	}
}
