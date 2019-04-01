#ifndef GRIDCREATINGCONDITIONLAPLACE_EDGESETINTERPOLATIONMODECOMMAND_H
#define GRIDCREATINGCONDITIONLAPLACE_EDGESETINTERPOLATIONMODECOMMAND_H

#include "../gridcreatingconditionlaplace.h"
#include "gridcreatingconditionlaplace_impl.h"

#include <QUndoCommand>

class GridCreatingConditionLaplace::EdgeSetInterpolationModeCommand : public QUndoCommand
{
public:
	EdgeSetInterpolationModeCommand(Impl::EdgeType edgeType, int edgeId, Impl::InterpolationType type, GridCreatingConditionLaplace::Impl* impl);

	void redo() override;
	void undo() override;

private:
	Impl::InterpolationType& targetType();

	Impl::InterpolationType m_newType;
	Impl::InterpolationType m_oldType;

	Impl::EdgeType m_edgeType;
	int m_edgeId;
	Impl* m_impl;
};

#endif // GRIDCREATINGCONDITIONLAPLACE_EDGESETINTERPOLATIONMODECOMMAND_H
