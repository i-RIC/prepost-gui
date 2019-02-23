#include "gridcreatingconditionlaplace_divisionsettingcommand.h"
#include "gridcreatingconditionlaplace_impl.h"

GridCreatingConditionLaplace::DivisionSettingCommand::DivisionSettingCommand(bool streamWise, int edgeId, int divNum, GridCreatingConditionLaplace::Impl* impl) :
	m_newDivNum {divNum},
	m_streamWise {streamWise},
	m_edgeId {edgeId},
	m_impl {impl}
{
	if (streamWise) {
		m_oldDivNum = impl->m_divCountsStreamWise[m_edgeId];
	} else {
		m_oldDivNum = impl->m_divCountsCrossSection[m_edgeId];
	}
}

void GridCreatingConditionLaplace::DivisionSettingCommand::redo()
{
	if (m_streamWise) {
		m_impl->m_divCountsStreamWise[m_edgeId] = m_newDivNum;
	}	else {
		m_impl->m_divCountsCrossSection[m_edgeId] = m_newDivNum;
	}
}

void GridCreatingConditionLaplace::DivisionSettingCommand::undo()
{
	if (m_streamWise) {
		m_impl->m_divCountsStreamWise[m_edgeId] = m_oldDivNum;
	}	else {
		m_impl->m_divCountsCrossSection[m_edgeId] = m_oldDivNum;
	}
}
