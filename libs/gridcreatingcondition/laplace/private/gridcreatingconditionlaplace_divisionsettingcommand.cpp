#include "gridcreatingconditionlaplace_divisionsettingcommand.h"
#include "gridcreatingconditionlaplace_impl.h"

GridCreatingConditionLaplace::DivisionSettingCommand::DivisionSettingCommand(bool streamWise, int edgeId, int divNum, DivisionMode mode, double commonRatio, bool thisLineOnly, GridCreatingConditionLaplace::Impl* impl) :
	m_newDivNum {divNum},
	m_newMode {mode},
	m_newCommonRatio {commonRatio},
	m_oldManualDivSetting {impl->m_manualDivSetting},
	m_streamWise {streamWise},
	m_edgeId {edgeId},
	m_thisLineOnly {thisLineOnly},
	m_impl {impl}
{
	if (streamWise) {
		auto tmpid = edgeId % (impl->m_ctrlPointCountI - 1);
		m_oldDivNum = impl->m_divCountsStreamWise[tmpid];
		if (thisLineOnly) {
			m_oldMode.push_back(impl->m_divModesStreamWise[edgeId]);
			m_oldCommonRatio.push_back(impl->m_divCommonRatiosStreamWise[edgeId]);
		} else {
			for (int j = 0; j < impl->m_ctrlPointCountJ; ++j) {
				int idx = tmpid + j * (impl->m_ctrlPointCountI - 1);
				m_oldMode.push_back(impl->m_divModesStreamWise[idx]);
				m_oldCommonRatio.push_back(impl->m_divCommonRatiosStreamWise[idx]);
			}
		}
	} else {
		auto tmpid = edgeId / impl->m_ctrlPointCountI;
		m_oldDivNum = impl->m_divCountsCrossSection[tmpid];
		if (thisLineOnly) {
			m_oldMode.push_back(impl->m_divModesCrossSection[edgeId]);
			m_oldCommonRatio.push_back(impl->m_divCommonRatiosCrossSection[edgeId]);
		} else {
			for (int i = 0; i < impl->m_ctrlPointCountI; ++i) {
				int idx = i + tmpid * impl->m_ctrlPointCountI;
				m_oldMode.push_back(impl->m_divModesCrossSection[idx]);
				m_oldCommonRatio.push_back(impl->m_divCommonRatiosCrossSection[idx]);
			}
		}
	}
}

void GridCreatingConditionLaplace::DivisionSettingCommand::redo()
{
	if (m_streamWise) {
		auto tmpid = m_edgeId % (m_impl->m_ctrlPointCountI - 1);
		m_impl->m_divCountsStreamWise[tmpid] = m_newDivNum;
		if (m_thisLineOnly) {
			m_impl->m_divModesStreamWise[m_edgeId] = m_newMode;
			m_impl->m_divCommonRatiosStreamWise[m_edgeId] = m_newCommonRatio;
		} else {
			for (int j = 0; j < m_impl->m_ctrlPointCountJ; ++j) {
				int idx = tmpid + j * (m_impl->m_ctrlPointCountI - 1);
				m_impl->m_divModesStreamWise[idx] = m_newMode;
				m_impl->m_divCommonRatiosStreamWise[idx] = m_newCommonRatio;
			}
		}
	}	else {
		auto tmpid = m_edgeId / m_impl->m_ctrlPointCountI;
		m_impl->m_divCountsCrossSection[tmpid] = m_newDivNum;
		if (m_thisLineOnly) {
			m_impl->m_divModesCrossSection[m_edgeId] = m_newMode;
			m_impl->m_divCommonRatiosCrossSection[m_edgeId] = m_newCommonRatio;
		} else {
			for (int i = 0; i < m_impl->m_ctrlPointCountI; ++i) {
				int idx = i + tmpid * m_impl->m_ctrlPointCountI;
				m_impl->m_divModesCrossSection[idx] = m_newMode;
				m_impl->m_divCommonRatiosCrossSection[idx] = m_newCommonRatio;
			}
		}
	}
	m_impl->m_manualDivSetting = true;
}

void GridCreatingConditionLaplace::DivisionSettingCommand::undo()
{
	if (m_streamWise) {
		auto tmpid = m_edgeId % (m_impl->m_ctrlPointCountI - 1);
		m_impl->m_divCountsStreamWise[tmpid] = m_oldDivNum;
		if (m_thisLineOnly) {
			m_impl->m_divModesStreamWise[m_edgeId] = m_oldMode[0];
			m_impl->m_divCommonRatiosStreamWise[m_edgeId] = m_oldCommonRatio[0];
		} else {
			for (int j = 0; j < m_impl->m_ctrlPointCountJ; ++j) {
				int idx = tmpid + j * (m_impl->m_ctrlPointCountI - 1);
				m_impl->m_divModesStreamWise[idx] = m_oldMode[j];
				m_impl->m_divCommonRatiosStreamWise[idx] = m_oldCommonRatio[j];
			}
		}
	}	else {
		auto tmpid = m_edgeId / m_impl->m_ctrlPointCountI;
		m_impl->m_divCountsCrossSection[tmpid] = m_oldDivNum;
		if (m_thisLineOnly) {
			m_impl->m_divModesCrossSection[m_edgeId] = m_oldMode[0];
			m_impl->m_divCommonRatiosCrossSection[m_edgeId] = m_oldCommonRatio[0];
		} else {
			for (int i = 0; i < m_impl->m_ctrlPointCountI; ++i) {
				int idx = i + tmpid * m_impl->m_ctrlPointCountI;
				m_impl->m_divModesCrossSection[idx] = m_oldMode[i];
				m_impl->m_divCommonRatiosCrossSection[idx] = m_oldCommonRatio[i];
			}
		}
	}
	m_impl->m_manualDivSetting = m_oldManualDivSetting;
}
