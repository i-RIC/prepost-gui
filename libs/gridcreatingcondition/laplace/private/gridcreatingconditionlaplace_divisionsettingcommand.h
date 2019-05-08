#ifndef GRIDCREATINGCONDITIONLAPLACE_DIVISIONSETTINGCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_DIVISIONSETTINGCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QUndoCommand>

class GridCreatingConditionLaplace::DivisionSettingCommand : public QUndoCommand
{
public:
	DivisionSettingCommand(bool streamWise, int edgeId, int divNum, DivisionMode mode, double commonRatio, bool thisLineOnly, GridCreatingConditionLaplace::Impl* impl);

	void redo() override;
	void undo() override;

private:
	int m_newDivNum;
	DivisionMode m_newMode;
	double m_newCommonRatio;

	int m_oldDivNum;
	std::vector<DivisionMode> m_oldMode;
	std::vector<double> m_oldCommonRatio;
	bool m_oldManualDivSetting;

	bool m_streamWise;
	int m_edgeId;
	bool m_thisLineOnly;
	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_DIVISIONSETTINGCOMMAND_H
