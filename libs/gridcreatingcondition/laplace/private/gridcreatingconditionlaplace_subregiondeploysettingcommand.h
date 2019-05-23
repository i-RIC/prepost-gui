#ifndef GRIDCREATINGCONDITIONLAPLACE_SUBREGIONDEPLOYSETTINGCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_SUBREGIONDEPLOYSETTINGCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QUndoCommand>

class GridCreatingConditionLaplace::SubRegionDeploySettingCommand : public QUndoCommand
{
public:
	SubRegionDeploySettingCommand(const GridCreatingConditionLaplace::DeployParameter& param, int index, GridCreatingConditionLaplace::Impl* impl);
	void redo() override;
	void undo() override;

private:
	GridCreatingConditionLaplace::DeployParameter m_newParameter;
	GridCreatingConditionLaplace::DeployParameter m_oldParameter;

	int m_index;
	bool m_oldManualDivSetting;
	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_SUBREGIONDEPLOYSETTINGCOMMAND_H
