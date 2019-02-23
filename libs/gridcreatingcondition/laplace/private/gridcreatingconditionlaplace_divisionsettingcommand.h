#ifndef GRIDCREATINGCONDITIONLAPLACE_DIVISIONSETTINGCOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_DIVISIONSETTINGCOMMAND_H

#include "../gridcreatingconditionlaplace.h"

#include <QUndoCommand>

class GridCreatingConditionLaplace::DivisionSettingCommand : public QUndoCommand
{
public:
	DivisionSettingCommand(bool streamWise, int edgeId, int divNum, GridCreatingConditionLaplace::Impl* impl);

	void redo() override;
	void undo() override;

private:
	int m_newDivNum;
	int m_oldDivNum;

	bool m_streamWise;
	int m_edgeId;
	GridCreatingConditionLaplace::Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_DIVISIONSETTINGCOMMAND_H
