#include "gridcreatingconditionlaplace_deploysettingcommand.h"

GridCreatingConditionLaplace::DeploySettingCommand::DeploySettingCommand(bool streamWise, int edgeId, DivisionMode mode, double commonRatio, GridCreatingConditionLaplace::Impl* impl) :
	m_newMode {mode},
	m_newCommonRatio {commonRatio},
	m_oldManualDivSetting {impl->m_manualDivSetting},
	m_streamWise {streamWise},
	m_edgeId {edgeId},
	m_impl {impl}
{
	if (m_streamWise) {
		m_oldMode = impl->m_divModesStreamWise[edgeId];
		m_oldCommonRatio =impl->m_divCommonRatiosStreamWise[edgeId];
	} else {
		m_oldMode = impl->m_divModesCrossSection[edgeId];
		m_oldCommonRatio =impl->m_divCommonRatiosCrossSection[edgeId];
	}
}

void GridCreatingConditionLaplace::DeploySettingCommand::redo()
{
	if (m_streamWise) {
		m_impl->m_divModesStreamWise[m_edgeId] = m_newMode;
		m_impl->m_divCommonRatiosStreamWise[m_edgeId] = m_newCommonRatio;
	} else {
		m_impl->m_divModesCrossSection[m_edgeId] = m_newMode;
		m_impl->m_divCommonRatiosCrossSection[m_edgeId] = m_newCommonRatio;
	}
	m_impl->m_manualDivSetting = true;
}

void GridCreatingConditionLaplace::DeploySettingCommand::undo()
{
	if (m_streamWise) {
		m_impl->m_divModesStreamWise[m_edgeId] = m_oldMode;
		m_impl->m_divCommonRatiosStreamWise[m_edgeId] = m_oldCommonRatio;
	} else {
		m_impl->m_divModesCrossSection[m_edgeId] = m_oldMode;
		m_impl->m_divCommonRatiosCrossSection[m_edgeId] = m_oldCommonRatio;
	}
	m_impl->m_manualDivSetting = m_oldManualDivSetting;
}
