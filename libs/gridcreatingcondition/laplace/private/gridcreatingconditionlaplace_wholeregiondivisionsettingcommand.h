#ifndef GRIDCREATINGCONDITIONLAPLACE_WHOLEREGIONDIVISIONSETTINGCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_WHOLEREGIONDIVISIONSETTINGCOMMAND_H

#include "../gridcreatingconditionlaplace.h"
#include "gridcreatingconditionlaplace_impl.h"

#include <QUndoCommand>

class GridCreatingConditionLaplace::WholeRegionDivisionSettingCommand : public QUndoCommand
{
public:
	WholeRegionDivisionSettingCommand(const std::vector<int>& streamWiseDivCounts, const std::vector<int>& crossSectionDivCounts, GridCreatingConditionLaplace::Impl* impl);

	void redo() override;
	void undo() override;

private:
	std::vector<int> m_newDivCountsStreamWise;
	std::vector<int> m_newDivCountsCrossSection;

	std::vector<int> m_oldDivCountsStreamWise;
	std::vector<int> m_oldDivCountsCrossSection;
	std::vector<DivisionMode> m_oldDivModesStreamWise;
	std::vector<DivisionMode> m_oldDivModesCrossSection;

	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_WHOLEREGIONDIVISIONSETTINGCOMMAND_H
