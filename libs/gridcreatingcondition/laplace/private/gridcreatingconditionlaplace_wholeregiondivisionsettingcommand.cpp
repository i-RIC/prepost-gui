#include "gridcreatingconditionlaplace_wholeregiondivisionsettingcommand.h"

GridCreatingConditionLaplace::WholeRegionDivisionSettingCommand::WholeRegionDivisionSettingCommand(const std::vector<int>& streamWiseDivCounts, const std::vector<int>& crossSectionDivCounts, GridCreatingConditionLaplace::Impl* impl) :
	m_newDivCountsStreamWise (streamWiseDivCounts),
	m_newDivCountsCrossSection (crossSectionDivCounts),
	m_oldDivCountsStreamWise (impl->m_divCountsStreamWise),
	m_oldDivCountsCrossSection (impl->m_divCountsCrossSection),
	m_oldDivModesStreamWise (impl->m_divModesStreamWise),
	m_oldDivModesCrossSection (impl->m_divModesCrossSection),
	m_impl {impl}
{}

void GridCreatingConditionLaplace::WholeRegionDivisionSettingCommand::redo()
{
	m_impl->m_divCountsStreamWise = m_newDivCountsStreamWise;
	m_impl->m_divCountsCrossSection = m_newDivCountsCrossSection;

	for (int i = 0; i < m_impl->m_divModesStreamWise.size(); ++i) {
		m_impl->m_divModesStreamWise[i] = DivisionMode::Auto;
	}
	for (int i = 0; i < m_impl->m_divModesCrossSection.size(); ++i) {
		m_impl->m_divModesCrossSection[i] = DivisionMode::Auto;
	}
}
void GridCreatingConditionLaplace::WholeRegionDivisionSettingCommand::undo()
{
	m_impl->m_divCountsStreamWise = m_oldDivCountsStreamWise;
	m_impl->m_divCountsCrossSection = m_oldDivCountsCrossSection;

	m_impl->m_divModesStreamWise = m_oldDivModesStreamWise;
	m_impl->m_divModesCrossSection = m_oldDivModesCrossSection;
}

