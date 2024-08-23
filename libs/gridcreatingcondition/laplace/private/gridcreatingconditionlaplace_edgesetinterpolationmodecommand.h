#ifndef GRIDCREATINGCONDITIONLAPLACE_EDGESETINTERPOLATIONMODECOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_EDGESETINTERPOLATIONMODECOMMAND_H

#include "../gridcreatingconditionlaplace.h"
#include "gridcreatingconditionlaplace_impl.h"

#include <QUndoCommand>

class GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand : public QUndoCommand
{
public:
	EdgeSetInterpolationModeCommand(Impl::EdgeType edgeType, std::vector<int> edgeIds, InterpolationType type, GridCreatingConditionLaplace::Impl* impl);

	void redo() override;
	void undo() override;

private:
	InterpolationType& targetType(int id);

	std::vector<InterpolationType> m_newTypes;
	std::vector<InterpolationType> m_oldTypes;

	Impl::EdgeType m_edgeType;
	std::vector<int> m_edgeIds;

	Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_EDGESETINTERPOLATIONMODECOMMAND_H
