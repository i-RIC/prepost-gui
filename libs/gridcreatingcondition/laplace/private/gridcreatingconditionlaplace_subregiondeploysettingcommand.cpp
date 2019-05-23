#include "gridcreatingconditionlaplace_subregiondeploysettingcommand.h"
#include "gridcreatingconditionlaplace_impl.h"

GridCreatingConditionLaplace::SubRegionDeploySettingCommand::SubRegionDeploySettingCommand(const GridCreatingConditionLaplace::DeployParameter& param, int index, GridCreatingConditionLaplace::Impl* impl) :
	m_newParameter {param},
	m_index {index},
	m_oldManualDivSetting {impl->m_manualDivSetting},
	m_impl {impl}
{
	m_oldParameter = impl->m_subRegionDeployParameters.at(index);
}

void GridCreatingConditionLaplace::SubRegionDeploySettingCommand::redo()
{
	m_impl->m_subRegionDeployParameters[m_index] = m_newParameter;
	m_impl->m_manualDivSetting = true;
}

void GridCreatingConditionLaplace::SubRegionDeploySettingCommand::undo()
{
	m_impl->m_subRegionDeployParameters[m_index] = m_oldParameter;
	m_impl->m_manualDivSetting = m_oldManualDivSetting;
}
