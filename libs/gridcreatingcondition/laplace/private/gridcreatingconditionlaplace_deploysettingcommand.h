#ifndef GRIDCREATINGCONDITIONLAPLACE_DEPLOYSETTINGCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_DEPLOYSETTINGCOMMAND_H

#include "../gridcreatingconditionlaplace.h"
#include "gridcreatingconditionlaplace_impl.h"

#include <QUndoCommand>

class GridCreatingConditionLaplace::DeploySettingCommand : public QUndoCommand
{
public:
	DeploySettingCommand(bool streamWise, int edgeId, DivisionMode mode, double commonRatio, GridCreatingConditionLaplace::Impl* impl);

	void redo() override;
	void undo() override;

private:
	DivisionMode m_newMode;
	double m_newCommonRatio;

	DivisionMode m_oldMode;
	double m_oldCommonRatio;
	bool m_oldManualDivSetting;

	bool m_streamWise;
	int m_edgeId;
	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_DEPLOYSETTINGCOMMAND_H
